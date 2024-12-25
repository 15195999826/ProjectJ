// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardExecuteHelper.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/ProjectJGameContext.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJItem.h"
#include "Game/Card/ProjectJLandmark.h"
#include "Game/Card/ProjectJUtility.h"
#include "ProjectJ/ProjectJGameplayTags.h"

void UProjectJCardExecuteHelper::GiveCardDisplayCondition(int32 InCardID, const FName& InConditionTag,
                                                          const FString& InConditionValue)
{
}

int32 UProjectJCardExecuteHelper::SpawnCharacter(const FName& InRowName)
{
	FVector RandomLocation;
	auto Find = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager->RandomEmptyLocation(RandomLocation);
	return  SpawnCharacterAtLocation(InRowName, RandomLocation, !Find);
}

int32 UProjectJCardExecuteHelper::SpawnCharacterAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto NewCard =  IntervalSpawnNewCard(ContextSystem, InLocation, InRowName, EProjectJCardType::Character, EProjectJItemType::None);
	IntervalScaleSpawnNewCard(NewCard, ContextSystem, Relayout);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::PopupCharacter(const FName& InRowName)
{
	UProjectJContextSystem* ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto ExecAreaLocation = ContextSystem->ExecuteArea->GetActorLocation();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, ExecAreaLocation, InRowName, EProjectJCardType::Character, EProjectJItemType::None);
	IntervalPopupNewCard(NewCard, ContextSystem);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::SpawnLandmark(const FName& InRowName)
{
	FVector RandomLocation;
	auto Find = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager->RandomEmptyLocation(RandomLocation);
	return SpawnLandmarkAtLocation(InRowName, RandomLocation, !Find);
}

int32 UProjectJCardExecuteHelper::SpawnLandmarkAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, InLocation, InRowName, EProjectJCardType::Landmark, EProjectJItemType::None);
	IntervalScaleSpawnNewCard(NewCard, ContextSystem, Relayout);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::PopupLandmark(const FName& InRowName)
{
	UProjectJContextSystem* ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto ExecAreaLocation = ContextSystem->ExecuteArea->GetActorLocation();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, ExecAreaLocation, InRowName, EProjectJCardType::Landmark, EProjectJItemType::None);
	IntervalPopupNewCard(NewCard, ContextSystem);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::SpawnUtility(const FName& InRowName)
{
	FVector RandomLocation;
	auto Find = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager->RandomEmptyLocation(RandomLocation);
	return SpawnUtilityAtLocation(InRowName, RandomLocation, !Find);
}

int32 UProjectJCardExecuteHelper::SpawnUtilityAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, InLocation, InRowName, EProjectJCardType::Utility, EProjectJItemType::None);
	IntervalScaleSpawnNewCard(NewCard, ContextSystem, Relayout);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::PopupUtility(const FName& InRowName)
{
	UProjectJContextSystem* ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto ExecAreaLocation = ContextSystem->ExecuteArea->GetActorLocation();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, ExecAreaLocation, InRowName, EProjectJCardType::Utility, EProjectJItemType::None);
	IntervalPopupNewCard(NewCard, ContextSystem);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::SpawnItem(const EProjectJItemType InItemType, const FName& InRowName)
{
	FVector RandomLocation;
	auto Find = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager->RandomEmptyLocation(RandomLocation);
	return SpawnItemAtLocation(InItemType, InRowName, RandomLocation, !Find);
}

int32 UProjectJCardExecuteHelper::SpawnItemAtLocation(const EProjectJItemType InItemType, const FName& InRowName,
	const FVector& InLocation, bool Relayout)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, InLocation, InRowName, EProjectJCardType::Item, InItemType);
	IntervalScaleSpawnNewCard(NewCard, ContextSystem, Relayout);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::PopupItem(const EProjectJItemType InItemType, const FName& InRowName)
{
	UProjectJContextSystem* ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto ExecAreaLocation = ContextSystem->ExecuteArea->GetActorLocation();
	auto NewCard = IntervalSpawnNewCard(ContextSystem, ExecAreaLocation, InRowName, EProjectJCardType::Item, InItemType);
	IntervalPopupNewCard(NewCard, ContextSystem);
	return NewCard->ID;
}

int32 UProjectJCardExecuteHelper::SpawnSpell(const FName& InSpellTag, const FVector& InLocation, UProjectJContextSystem* InContextSystem)
{
	auto NewCard = IntervalSpawnNewCard(InContextSystem, InLocation, InSpellTag, EProjectJCardType::Spell, EProjectJItemType::None);
	IntervalScaleSpawnNewCard(NewCard, InContextSystem, false);
	return NewCard->ID;
}

AProjectJCardBase* UProjectJCardExecuteHelper::IntervalSpawnNewCard(UProjectJContextSystem* InContextSystem, const FVector& InLocation, const FName& InRowName,
                                                                    EProjectJCardType InType, EProjectJItemType InItemType)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnNewCard, RowName: %s, CardType: %d, ItemType: %d"), *InRowName.ToString(), (int32)InType, (int32)InItemType);
	AProjectJCardBase* NewCard;
	switch (InType) {
		case EProjectJCardType::Character:
			{
				NewCard = InContextSystem->CreateCharacter(InRowName);
			}
		break;
		case EProjectJCardType::Landmark:
			{
				NewCard = InContextSystem->CreateLandMark(InRowName);
			}
		break;
		case EProjectJCardType::Utility:
			{
				NewCard = InContextSystem->CreateUtility(InRowName);
			}
		break;
		case EProjectJCardType::Item:
			{
				NewCard = InContextSystem->CreateItem(InRowName, InItemType);
			}
		break;
		case EProjectJCardType::Spell:
			{
				NewCard = InContextSystem->CreateSpell(InRowName);
			}
			break;
		default:
			{
				UE_LOG(LogTemp, Error, TEXT("不支持生成的 CardType %d"), (int32)InType);
				return nullptr;
			}
	}
	NewCard->SetActorLocation(InLocation);
	check(InContextSystem->GameContext->InDungeonCards.Contains(NewCard) == false);
	InContextSystem->GameContext->InDungeonCards.Add(NewCard);
	check(InContextSystem->GameContext->InDungeonCardsMap.Contains(NewCard->ID) == false);
	InContextSystem->GameContext->InDungeonCardsMap.Add(NewCard->ID, NewCard);
	return NewCard;
}

void UProjectJCardExecuteHelper::IntervalPopupNewCard(AProjectJCardBase* InCard, UProjectJContextSystem* InContextSystem)
{
	auto CardLocation = InCard->GetActorLocation();
	float PopupDuration = 0.8f;
	auto DesiredLocation = CardLocation + FVector(200.f, 200.f, 10.f);
	InCard->PopupCard(CardLocation, DesiredLocation, PopupDuration);
	// 0.8s内，禁止抓取卡牌
	auto AllCard = InContextSystem->GameContext->InDungeonCards;
	for (auto& Card : AllCard)
	{
		Card->CanDrag = false;
	}
	FTimerHandle UnlockTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(UnlockTimerHandle, [InCard, InContextSystem, AllCard, DesiredLocation]() {
		auto GroundDesiredLocation = DesiredLocation;
		GroundDesiredLocation.Z = 0.f;
		InContextSystem->CardLayoutManager->PlaceCardAndRelayout(InCard, 0.1f, GroundDesiredLocation);
		for (auto& Card : AllCard)
		{
			Card->CanDrag = true;
		}
	}, PopupDuration - 0.033f, false);
}

void UProjectJCardExecuteHelper::IntervalScaleSpawnNewCard(AProjectJCardBase* InCard,
	const UProjectJContextSystem* InContextSystem, bool Relayout)
{
	if (Relayout)
	{
		auto CardLocation = InCard->GetActorLocation();
		CardLocation.Z = 0.f;
		InContextSystem->CardLayoutManager->PlaceCardAndRelayout(InCard, 0.1f, CardLocation);
	}

	InCard->ShowCard();
}

void UProjectJCardExecuteHelper::DestroyCard(int32 InCardID) const
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto Card = ContextSystem->GameContext->InDungeonCardsMap.FindRef(InCardID);
	check(Card);
	ContextSystem->GameContext->InDungeonCardsMap.Remove(InCardID);
	ContextSystem->GameContext->InDungeonCards.Remove(Card);
	ContextSystem->RecycleByID(InCardID);
}

int32 UProjectJCardExecuteHelper::Roll(const FName& InTagName) const
{
	// 目前直接返回角色属性， 不知道Roll的规则， 然后UI上播放Roll点动画， 需要玩家点击后跳过， 然后逻辑帧才会继续Tick
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto UsingCharacterID = ContextSystem->GameContext->UsingCharacterID;
	auto UsingCharacter = ContextSystem->UsingCharacters.FindRef(UsingCharacterID);
	check(UsingCharacter.IsValid());
	auto AttrTag = UGameplayTagsManager::Get().RequestGameplayTag(InTagName);
	check(AttrTag.IsValid());
	int32 Ret = 0;
	if (ProjectJGameplayTags::Tag2Attribute.Contains(AttrTag))
	{
		auto GetAttributeFunc = ProjectJGameplayTags::Tag2Attribute.FindRef(AttrTag);
		auto AttrValue = UsingCharacter->GetAbilitySystemComponent()->GetNumericAttribute(GetAttributeFunc());
		Ret = AttrValue;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Roll Tag %s 未找到对应的属性"), *InTagName.ToString());
	}

	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->PostRollResult.Broadcast(Ret);
	return Ret;
}
