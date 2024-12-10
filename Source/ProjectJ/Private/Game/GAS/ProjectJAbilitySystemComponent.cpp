// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJAbilitySystemComponent.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJBattleManager.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJGameplayEffectContext.h"
#include "Game/GAS/ProjectJLuaGameplayAbility.h"
#include "ProjectJ/ProjectJGameplayTags.h"

UProjectJAbilitySystemComponent::UProjectJAbilitySystemComponent()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UProjectJAbilitySystemComponent::OnGEAppliedToSelf);
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UProjectJAbilitySystemComponent::OnGERemoved);
}

void UProjectJAbilitySystemComponent::HandleMontagePostEvent(FGameplayTag EventTag, FProjectJMontageEventData EventData)
{
	if (FProjectJMontagePostDelegate* Delegate = MontageEventCallbacks.Find(EventTag))
	{
		// Make a copy before broadcasting to prevent memory stomping
		FProjectJMontagePostDelegate DelegateCopy = *Delegate;
		DelegateCopy.Broadcast(&EventData);
	}
}

bool UProjectJAbilitySystemComponent::CustomRemoveActiveGameplayEffect(FActiveGameplayEffectHandle Handle,
	const TArray<int32> LayerIDs)
{
	auto GEContext = static_cast<FProjectJGameplayEffectContext*>(GetActiveGameplayEffect(Handle)->Spec.GetContext().Get());
	GEContext->SetRemovingLayerIDs(LayerIDs);
	auto Num = LayerIDs.Num();
	return RemoveActiveGameplayEffect(Handle, Num);
}

void UProjectJAbilitySystemComponent::OnGEAppliedToSelf(UAbilitySystemComponent* SourceASC,
                                                        const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
#if WITH_EDITOR || UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	FGameplayTagContainer DefAssetTags = GameplayEffectSpec.Def->GetAssetTags();
	if (DefAssetTags.HasTag(ProjectJGameplayTags::Debug_Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnGEAppliedToSelf] BuffName: %s, Self: %s"), *GameplayEffectSpec.Def.GetName(), *GetOwner()->GetName());
	}
#endif
	// 
	
	if (GameplayEffectSpec.Def->DurationPolicy != EGameplayEffectDurationType::Instant &&
		GameplayEffectSpec.Def->StackingType != EGameplayEffectStackingType::None &&
		!CustomStackedGESpecMap.Contains(ActiveGameplayEffectHandle))
	{
		auto GrantedTags = GameplayEffectSpec.Def->GetGrantedTags();
		const auto& FirstGrantedTag = GrantedTags.Num() > 0 ? GrantedTags.GetByIndex(0) : FGameplayTag();
		// Todo: 暂时使用First GrantedTag来判断是否是我们自定义的Buff, 未来可能可以加专门的Tag, 防止一个GE配置了多个GrantedTag
		UE_LOG(LogTemp, Warning, TEXT("[%s]AddListener, BuffName: %s, Self: %s, Handle: %s"),
		       *FirstGrantedTag.GetTagName().ToString(), *GameplayEffectSpec.Def.GetName(), *GetOwner()->GetName(),
		       *ActiveGameplayEffectHandle.ToString());
		// 监听GE的StackCount的变化
		if (ProjectJGameplayTags::GoodFeatureTags.Contains(FirstGrantedTag) ||
			ProjectJGameplayTags::BadFeatureTags.Contains(FirstGrantedTag) ||
			ProjectJGameplayTags::NeutralFeatureTags.Contains(FirstGrantedTag))
		{
			check(Handle2FeatureTagMap.Contains(ActiveGameplayEffectHandle) == false);
			Handle2FeatureTagMap.Add(ActiveGameplayEffectHandle, FirstGrantedTag);
			FProjectJCustomStackedGESpec StackedGECustomSpec;
			CustomStackedGESpecMap.Add(ActiveGameplayEffectHandle, StackedGECustomSpec);
			CustomStackedGESpecMap[ActiveGameplayEffectHandle].StackCountChangedHandle = OnGameplayEffectStackChangeDelegate(ActiveGameplayEffectHandle)->AddUObject(
											this, &UProjectJAbilitySystemComponent::OnGameplayEffectStackChange);

			// 首次主动调用一次， 层数GE， 获得N层，调用N此Add
			int32 StackCount = GameplayEffectSpec.GetStackCount();
			check(StackCount == 1);
			OnGameplayEffectStackChange(ActiveGameplayEffectHandle, StackCount, 0);
		}
	}
}

void UProjectJAbilitySystemComponent::OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
#if WITH_EDITOR || UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	FGameplayTagContainer DefAssetTags = ActiveGameplayEffect.Spec.Def->GetAssetTags();
	if (DefAssetTags.HasTag(ProjectJGameplayTags::Debug_Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnGERemoved] BuffName: %s, Self: %s"), *ActiveGameplayEffect.Spec.Def.GetName(), *GetOwner()->GetName());
	}
#endif
	if (CustomStackedGESpecMap.Contains(ActiveGameplayEffect.Handle))
	{
		check(Handle2FeatureTagMap.Contains(ActiveGameplayEffect.Handle));
		UE_LOG(LogTemp, Warning, TEXT("[%s]RemoveCustomSpec, Self: %s, Handle: %s"),
		       *ActiveGameplayEffect.Spec.Def.GetName(), *GetOwner()->GetName(),
		       *ActiveGameplayEffect.Handle.ToString());
		CustomStackedGESpecMap[ActiveGameplayEffect.Handle].StackCountChangedHandle.Reset();
		CustomStackedGESpecMap.Remove(ActiveGameplayEffect.Handle);
		Handle2FeatureTagMap.Remove(ActiveGameplayEffect.Handle);
	}

	// Todo: 考虑其它非战斗的功能是否要用ASC来实现
}

void UProjectJAbilitySystemComponent::OnGameplayEffectStackChange(FActiveGameplayEffectHandle InGEHandle,
	int32 NewStackCount, int32 OldStackCount)
{
	if (NewStackCount == OldStackCount)
	{
		return;
	}

	const auto& Spec = GetActiveGameplayEffect(InGEHandle)->Spec;
	auto GEContext = static_cast<FProjectJGameplayEffectContext*>(Spec.GetContext().Get());
	auto Duration = GEContext->GetDuration();
	auto CurrentRound = GetWorld()->GetSubsystem<UProjectJContextSystem>()->BattleManager->GetRound();
	auto DT = NewStackCount - OldStackCount;
	UE_LOG(LogTemp, Warning, TEXT("StackCountChange, BuffName: %s, Self: %s, Handle: %s, StackDT: %d, 新获得的层数的时间Duration: %d"),
		   *Spec.Def.GetName(), *GetOwner()->GetName(), *InGEHandle.ToString(), DT, Duration);
	check(CustomStackedGESpecMap.Contains(InGEHandle));
	
	if (DT > 0)
	{
		// 由于不存在一次性Apply多层GE的函数，每次GEStack Count 增加时， 数值只能是1
		check(DT == 1);
		auto ID = ++CustomStackedGESpecMap[InGEHandle].GLayerID;
		FProjectJGELayerData LayerData;
		LayerData.LayerID = ID;
		LayerData.GetRound = CurrentRound;
		LayerData.Duration = Duration;
		CustomStackedGESpecMap[InGEHandle].EachLayerData.Add(LayerData);
	}
	else
	{
		if (CustomStackedGESpecMap[InGEHandle].StackCountReduceAtRoundMap.Contains(CurrentRound))
		{
			CustomStackedGESpecMap[InGEHandle].StackCountReduceAtRoundMap[CurrentRound] += DT;
		}
		else
		{
			CustomStackedGESpecMap[InGEHandle].StackCountReduceAtRoundMap.Add(CurrentRound, DT);
		}
	}
	
	// Todo: 临时Debug， 目前只会增加StackCountAddAtRoundMap， 不会主动去清理， 因此记录总数即可
	int TotalStackCount = CustomStackedGESpecMap[InGEHandle].EachLayerData.Num();
	for (const auto& Pair : CustomStackedGESpecMap[InGEHandle].StackCountReduceAtRoundMap)
	{
		TotalStackCount += Pair.Value;
	}
	
	if (TotalStackCount != NewStackCount)
	{
		UE_LOG(LogTemp, Error, TEXT("数据记录错误, TotalStackCount: %d, NewStackCount: %d"), TotalStackCount, NewStackCount);
	}
	// Todo: 代码保留，获取自定义的Context的方式
}
