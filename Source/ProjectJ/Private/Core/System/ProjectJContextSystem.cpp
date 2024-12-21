// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/System/ProjectJContextSystem.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Components/BoxComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Game/ProjectJCardBackendSystem.h"
#include "Game/ProjectJEffectActor.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/ProjectJNavPointActor.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJLandmark.h"
#include "Game/Card/ProjectJSpell.h"
#include "Game/GAS/ProjectJLuaGameplayAbility.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJItem.h"
#include "Types/ProjectJCharacterConfig.h"
#include "Types/ProjectJLandmarkConfig.h"

AProjectJSpell* UProjectJContextSystem::GetSpell()
{
	AProjectJSpell* Ret;
	if (SpellPool.Num() > 0)
	{
		Ret = SpellPool.Pop();
	}
	else
	{
		auto SpellClass = GetDefault<UProjectJGeneralSettings>()->SpellClass;
		Ret = GetWorld()->SpawnActor<AProjectJSpell>(SpellClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingSpells.Add(Ret->ID, Ret);
	return Ret;
}

AProjectJCharacter* UProjectJContextSystem::GetCharacter()
{
	AProjectJCharacter* Ret;
	if (CharacterPool.Num() > 0)
	{
		Ret = CharacterPool.Pop();
	}
	else
	{
		auto GeneralSettings = GetDefault<UProjectJGeneralSettings>();
		auto CharacterClass = GeneralSettings->CharacterClass;
		Ret = GetWorld()->SpawnActor<AProjectJCharacter>(CharacterClass);

		// 赋予常驻的被动技能
		auto LuaAbilityClass = GeneralSettings->LuaAbilityClass;
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(LuaAbilityClass, 1, 0, Ret);
		auto ASC = Ret->GetAbilitySystemComponent();
		auto SpecHandle = ASC->GiveAbilityAndActivateOnce(Spec);
		Ret->LuaAbility = Cast<UProjectJLuaGameplayAbility>(ASC->FindAbilitySpecFromHandle(SpecHandle)->GetPrimaryInstance());
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingCharacters.Add(Ret->ID, Ret);
#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingCharacters"));
#endif
	return Ret;
}

AProjectJLandmark* UProjectJContextSystem::GetLandMark()
{
	AProjectJLandmark* Ret;
	if (LandmarkPool.Num() > 0)
	{
		Ret = LandmarkPool.Pop();
	}
	else
	{
		auto LandmarkClass = GetDefault<UProjectJGeneralSettings>()->LandmarkClass;
		Ret = GetWorld()->SpawnActor<AProjectJLandmark>(LandmarkClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingLandmarks.Add(Ret->ID, Ret);

#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingLandmarks"));
#endif
	return Ret;
}

AProjectJItem* UProjectJContextSystem::GetItem()
{
	AProjectJItem* Ret;
	if (ItemPool.Num() > 0)
	{
		Ret = ItemPool.Pop();
	}
	else
	{
		auto ItemClass = GetDefault<UProjectJGeneralSettings>()->ItemClass;
		Ret = GetWorld()->SpawnActor<AProjectJItem>(ItemClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingItems.Add(Ret->ID, Ret);

#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingItems"));
#endif
	return Ret;
}

AProjectJNavPointActor* UProjectJContextSystem::GetNavPoint()
{
	AProjectJNavPointActor* Ret;
	if (NavPointPool.Num() > 0)
	{
		Ret = NavPointPool.Pop();
	}
	else
	{
		auto NavPointClass = GetDefault<UProjectJGeneralSettings>()->NavPointClass;
		Ret = GetWorld()->SpawnActor<AProjectJNavPointActor>(NavPointClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingNavPoints.Add(Ret->ID, Ret);
#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingNavPoints"));
#endif
	return Ret;
}

TArray<TObjectPtr<AProjectJCardBase>> UProjectJContextSystem::GetUsingCards()
{
	TArray<TObjectPtr<AProjectJCardBase>> Ret;
	for (auto& Pair : UsingSpells)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingCharacters)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingLandmarks)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingItems)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Value.Get());
		}
	}

	return Ret;
}

TMap<int32, TObjectPtr<AProjectJCardBase>> UProjectJContextSystem::GetUsingCardsMap()
{
	TMap<int32, TObjectPtr<AProjectJCardBase>> Ret;
	for (auto& Pair : UsingSpells)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Key, Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingCharacters)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Key, Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingLandmarks)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Key, Pair.Value.Get());
		}
	}
	for (auto& Pair : UsingItems)
	{
		if (Pair.Value.IsValid())
		{
			Ret.Add(Pair.Key, Pair.Value.Get());
		}
	}

	return Ret;
}

AProjectJSpell* UProjectJContextSystem::CreateSpell(const FName& Config)
{
	AProjectJSpell* Spell = GetSpell();
	IProjectJCardInterface::Execute_BindConfig(Spell, Config);
	return Spell;
}

AProjectJCharacter* UProjectJContextSystem::CreateCharacter(const FName& Config)
{
	AProjectJCharacter* Character = GetCharacter();
	IProjectJCardInterface::Execute_BindConfig(Character, Config);
	auto ConfigRow = GetDefault<UProjectJDataTableSettings>()->CharacterTable.LoadSynchronous()->FindRow<FProjectJCharacterConfig>(Config, TEXT("CreateCharacter"));
	LuaExecutor->CreateCharacter(Character->ID, ConfigRow->LuaScriptName);
	// Todo: 在ItemSystem注册装备槽位
	// auto ItemSystem = GetWorld()->GetSubsystem<UProjectJCardBackendSystem>();
	// ItemSystem->RegisterCharacterLogicSlots(Character->ID);
	return Character;
}

AProjectJLandmark* UProjectJContextSystem::CreateLandMark(const FName& Config)
{
	AProjectJLandmark* LandMark = GetLandMark();
	IProjectJCardInterface::Execute_BindConfig(LandMark, Config);
	auto ConfigRow = GetDefault<UProjectJDataTableSettings>()->LandmarkTable.LoadSynchronous()->FindRow<FProjectJLandmarkConfig>(Config, TEXT("CreateLandMark"));
	LuaExecutor->CreateLandMark(LandMark->ID, ConfigRow->LuaScriptName);
	return LandMark;
}

AProjectJItem* UProjectJContextSystem::CreateItem(const FName& Config, EProjectJItemType InType)
{
	AProjectJItem* Item = GetItem();
	IProjectJCardInterface::Execute_BindConfig(Item, Config);

	// Todo: 创建lua侧脚本
	
	return Item;
}

AProjectJNavPointActor* UProjectJContextSystem::CreateNavPoint(const FProjectJNavPoint& Config)
{
	AProjectJNavPointActor* NavPoint = GetNavPoint();
	NavPoint->BindConfig(Config);
	return NavPoint;
}

void UProjectJContextSystem::RecycleByID(int32 InID)
{
	if (UsingSpells.Contains(InID))
	{
		RecycleSpell(UsingSpells[InID].Get());
	}
	else if (UsingCharacters.Contains(InID))
	{
		RecycleCharacter(UsingCharacters[InID].Get());
	}
	else if (UsingLandmarks.Contains(InID))
	{
		RecycleLandMark(UsingLandmarks[InID].Get());
	}
	else if (UsingItems.Contains(InID))
	{
		RecycleItem(UsingItems[InID].Get());
	}
	else if (UsingNavPoints.Contains(InID))
	{
		RecycleNavPoint(UsingNavPoints[InID].Get());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RecycleByID: Can't find ID: %d"), InID);
	}
}

void UProjectJContextSystem::RecycleNavPoint(AProjectJNavPointActor* NavPoint)
{
	UsingNavPoints.Remove(NavPoint->ID);
	NavPointPool.Add(NavPoint);
	GeneralOnRecycle(NavPoint);
#if WITH_EDITOR
	NavPoint->SetFolderPath(TEXT("NavPointPool"));
#endif
}

void UProjectJContextSystem::RecycleSpell(AProjectJSpell* Spell)
{
	// Detach
	if (Spell->GetAttachParentActor())
	{
		Spell->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	UsingSpells.Remove(Spell->ID);
	SpellPool.Add(Spell);
	GeneralOnRecycle(Spell);
#if WITH_EDITOR
	Spell->SetFolderPath(TEXT("SpellPool"));
#endif
}

void UProjectJContextSystem::RecycleCharacter(AProjectJCharacter* Character)
{
	LuaExecutor->RemoveCharacter(Character->ID);
	auto ItemSystem = GetWorld()->GetSubsystem<UProjectJCardBackendSystem>();
	ItemSystem->RemoveCharacterLogicSlots(Character->ID);

	// 清除除了LuaAbility以外的所有GameplayAbility
	// TArray<FGameplayAbilitySpecHandle> AllAbilities;
	// AbilitySystemComponent->GetAllAbilities(AllAbilities);
	// for (auto& Ability : AllAbilities)
	// {
	// 	if (Ability != LuaAbilityHandle)
	// 	{
	// 		AbilitySystemComponent->ClearAbility(Ability);
	// 	}
	// }
	//
	// LuaAbility->ResetAbility();
	//
	// // 清除全部GE	
	// FGameplayEffectQuery Query;
	// Query.CustomMatchDelegate.BindLambda([](const FActiveGameplayEffect& Effect)
	// {
	// 	FGameplayTagContainer AssetTags;
	// 	Effect.Spec.GetAllAssetTags(AssetTags);
	// 	bool NeverDelete = AssetTags.HasTagExact(AutoDWGameplayTags::Buff_NeverDelete.GetTag()) || Effect.Spec.GetDynamicAssetTags().HasTagExact(AutoDWGameplayTags::Buff_NeverDelete.GetTag());
	// 	return !NeverDelete;
	// });
	// AbilitySystemComponent->RemoveActiveEffects(Query);
	
	UsingCharacters.Remove(Character->ID);
	CharacterPool.Add(Character);
	GeneralOnRecycle(Character);
#if WITH_EDITOR
	Character->SetFolderPath(TEXT("CharacterPool"));
#endif
}

void UProjectJContextSystem::RecycleLandMark(AProjectJLandmark* LandMark)
{
	LuaExecutor->RemoveLandMark(LandMark->ID);
	UsingLandmarks.Remove(LandMark->ID);
	LandmarkPool.Add(LandMark);
	GeneralOnRecycle(LandMark);
#if WITH_EDITOR
	LandMark->SetFolderPath(TEXT("LandmarkPool"));
#endif
}

void UProjectJContextSystem::RecycleItem(AProjectJItem* Item)
{
	LuaExecutor->RemoveItem(Item->ID);
	UsingItems.Remove(Item->ID);
	ItemPool.Add(Item);
	GeneralOnRecycle(Item);
#if WITH_EDITOR
	Item->SetFolderPath(TEXT("ItemPool"));
#endif
}

void UProjectJContextSystem::GeneralOnGet(AActor* InActor)
{
	InActor->SetActorEnableCollision(true);
}

void UProjectJContextSystem::GeneralOnRecycle(AActor* InActor)
{
	InActor->SetActorEnableCollision(false);
	InActor->SetActorLocation(HiddenLocation);
}

AProjectJEffectActor* UProjectJContextSystem::GetEffectActor(TSubclassOf<UObject> InEffectClass)
{
	FName EffectName = InEffectClass->GetClass()->GetFName();
	// UE_LOG(LogTemp, Warning, TEXT("GetEffectActor: %s"), *EffectName.ToString());
	if (EffectActorPool.Contains(EffectName))
	{
		auto Pool = EffectActorPool[EffectName].Pool;
		if (Pool.Num() > 0)
		{
			auto EffectActor = Pool.Pop();
			return EffectActor;
		}
	}

	// 检查InSoftEffect是否是AProjectJEffectActor类型
	if (!InEffectClass->IsChildOf(AProjectJEffectActor::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("GetEffectActor: InSoftEffect is not AProjectJEffectActor"));
		return nullptr;
	}

	auto EffectActor = GetWorld()->SpawnActor<AProjectJEffectActor>(InEffectClass);
	return EffectActor;
}

void UProjectJContextSystem::RecycleEffectActor(AProjectJEffectActor* InEffectActor)
{
	FName EffectName = InEffectActor->GetClass()->GetFName();
	if (!EffectActorPool.Contains(EffectName))
	{
		EffectActorPool.Add(EffectName, FProjectJEffectContainer());
	}
	InEffectActor->SetActorEnableCollision(false);
	InEffectActor->SetActorLocation(HiddenLocation);
	InEffectActor->SetActorHiddenInGame(true);
	EffectActorPool[EffectName].Pool.Add(InEffectActor);
}

bool UProjectJContextSystem::IsOverlapExecuteArea(const FVector& InCardPosition) const
{
	FVector BlockAreaSize = ExecuteArea->BoxComponent->GetScaledBoxExtent();
	FVector BlockAreaCenter = ExecuteArea->BoxComponent->GetComponentLocation();
	BlockAreaCenter.Z = 0.f;

	auto GSettings = GetDefault<UProjectJGeneralSettings>();

	auto  HalfCardSize = GSettings->CardSize * 0.5f;

	// Check if the card's position is within the block area bounds
	bool bIsInXBounds = InCardPosition.X + HalfCardSize.X > BlockAreaCenter.X - BlockAreaSize.X &&
						InCardPosition.X - HalfCardSize.X < BlockAreaCenter.X + BlockAreaSize.X;
	bool bIsInYBounds = InCardPosition.Y + HalfCardSize.Y > BlockAreaCenter.Y - BlockAreaSize.Y &&
						InCardPosition.Y - HalfCardSize.Y < BlockAreaCenter.Y + BlockAreaSize.Y;

	return bIsInXBounds && bIsInYBounds;
}

bool UProjectJContextSystem::IsInExecuteArea(const FVector& InCardPosition) const
{
	FVector BlockAreaSize = ExecuteArea->BoxComponent->GetScaledBoxExtent();
	FVector BlockAreaCenter = ExecuteArea->BoxComponent->GetComponentLocation();
	BlockAreaCenter.Z = 0.f;
	// Check if the card's position is within the block area bounds
	bool bIsInXBounds = InCardPosition.X > BlockAreaCenter.X - BlockAreaSize.X &&
						InCardPosition.X < BlockAreaCenter.X + BlockAreaSize.X;
	bool bIsInYBounds = InCardPosition.Y> BlockAreaCenter.Y - BlockAreaSize.Y &&
						InCardPosition.Y< BlockAreaCenter.Y + BlockAreaSize.Y;

	return bIsInXBounds && bIsInYBounds;
}
