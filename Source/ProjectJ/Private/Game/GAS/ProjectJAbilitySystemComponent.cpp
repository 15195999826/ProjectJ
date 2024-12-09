// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJAbilitySystemComponent.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJBattleManager.h"
#include "Game/GAS/ProjectJGameplayEffectContext.h"
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

void UProjectJAbilitySystemComponent::OnGEAppliedToSelf(UAbilitySystemComponent* SourceASC,
                                                        const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer DefAssetTags = GameplayEffectSpec.Def->GetAssetTags();
#if WITH_EDITOR || UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	if (DefAssetTags.HasTag(ProjectJGameplayTags::Debug_Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnGEAppliedToSelf] BuffName: %s, Self: %s"), *GameplayEffectSpec.Def.GetName(), *GetOwner()->GetName());
	}
#endif
	// 默认读取AssetTag 的第0个， 作为代理的Key
	const auto& FirstAssetTag = DefAssetTags.Num() > 0 ? DefAssetTags.GetByIndex(0) : FGameplayTag();

	if (GameplayEffectSpec.Def->DurationPolicy != EGameplayEffectDurationType::Instant &&
		GameplayEffectSpec.Def->StackingType != EGameplayEffectStackingType::None &&
		!GEStackCountListenerMap.Contains(FirstAssetTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]AddListener, BuffName: %s, Self: %s, Handle: %s"),
		       *FirstAssetTag.GetTagName().ToString(), *GameplayEffectSpec.Def.GetName(), *GetOwner()->GetName(),
		       *ActiveGameplayEffectHandle.ToString());
		// 监听GE的StackCount的变化
		if (ProjectJGameplayTags::GoodFeatureTags.Contains(FirstAssetTag) ||
			ProjectJGameplayTags::BadFeatureTags.Contains(FirstAssetTag) ||
			ProjectJGameplayTags::NeutralFeatureTags.Contains(FirstAssetTag))
		{
			// Do something
			GEStackCountListenerMap.Add(FirstAssetTag,
			                            OnGameplayEffectStackChangeDelegate(ActiveGameplayEffectHandle)->AddUObject(
				                            this, &UProjectJAbilitySystemComponent::OnGameplayEffectStackChange));
		}
	}
}

void UProjectJAbilitySystemComponent::OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	FGameplayTagContainer DefAssetTags = ActiveGameplayEffect.Spec.Def->GetAssetTags();
#if WITH_EDITOR || UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	if (DefAssetTags.HasTag(ProjectJGameplayTags::Debug_Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnGERemoved] BuffName: %s, Self: %s"), *ActiveGameplayEffect.Spec.Def.GetName(), *GetOwner()->GetName());
	}
#endif

	auto FirstAssetTag = DefAssetTags.Num() > 0 ? DefAssetTags.GetByIndex(0) : FGameplayTag();
	if (GEStackCountListenerMap.Contains(FirstAssetTag))
	{
		GEStackCountListenerMap[FirstAssetTag].Reset();
		GEStackCountListenerMap.Remove(FirstAssetTag);
	}
}

void UProjectJAbilitySystemComponent::OnGameplayEffectStackChange(FActiveGameplayEffectHandle InGEHandle,
	int32 NewStackCount, int32 OldStackCount)
{
	if (NewStackCount == OldStackCount)
	{
		return;
	}
	
	auto CurrentRound = GetWorld()->GetSubsystem<UProjectJContextSystem>()->BattleManager->GetRound();
	auto GEContext = static_cast<FProjectJGameplayEffectContext*>(GetActiveGameplayEffect(InGEHandle)->Spec.GetContext().Get());
	GEContext->AddStackCountAtRound(CurrentRound, NewStackCount - OldStackCount);
}
