// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardExecuteHelper.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJItem.h"
#include "Game/Card/ProjectJLandmark.h"
#include "Game/Card/ProjectJUtility.h"

void UProjectJCardExecuteHelper::GiveCardDisplayCondition(int32 InCardID, const FName& InConditionTag,
	const FString& InConditionValue)
{
}

int32 UProjectJCardExecuteHelper::SpawnCharacter(const FName& InRowName)
{
	FVector RandomLocation;
	auto Find = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager->RandomEmptyLocation(RandomLocation);
	if (!Find)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnCharacter] 没有空位可以生成卡牌"));
		// 简单随机一个在Bounds内的位置
	}
	return SpawnCharacterAtLocation(InRowName, RandomLocation);
}

int32 UProjectJCardExecuteHelper::SpawnCharacterAtLocation(const FName& InRowName, const FVector& InLocation)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto NewCard =  IntervalSpawnNewCard(ContextSystem, InLocation, InRowName, EProjectJCardType::Character, EProjectJItemType::None);
	IntervalScaleSpawnNewCard(NewCard);
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
}

int32 UProjectJCardExecuteHelper::SpawnLandmarkAtLocation(const FName& InRowName, const FVector& InLocation)
{
}

int32 UProjectJCardExecuteHelper::PopupLandmark(const FName& InRowName)
{
}

int32 UProjectJCardExecuteHelper::SpawnUtility(const FName& InRowName)
{
}

int32 UProjectJCardExecuteHelper::SpawnUtilityAtLocation(const FName& InRowName, const FVector& InLocation)
{
}

int32 UProjectJCardExecuteHelper::PopupUtility(const FName& InRowName)
{
}

int32 UProjectJCardExecuteHelper::SpawnItem(const EProjectJItemType InItemType, const FName& InRowName)
{
}

int32 UProjectJCardExecuteHelper::SpawnItemAtLocation(const EProjectJItemType InItemType, const FName& InRowName,
	const FVector& InLocation)
{
}

int32 UProjectJCardExecuteHelper::PopupItem(const EProjectJItemType InItemType, const FName& InRowName)
{
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
		default:
			{
				UE_LOG(LogTemp, Error, TEXT("不支持生成的 CardType %d"), (int32)InType);
				return nullptr;
			}
	}
	NewCard->SetActorLocation(InLocation);

	return NewCard;
}

void UProjectJCardExecuteHelper::IntervalPopupNewCard(AProjectJCardBase* InCard, UProjectJContextSystem* InContextSystem)
{
	auto CardLocation = InCard->GetActorLocation();
	float PopupDuration = 0.8f;
	auto DesiredLocation = CardLocation + FVector(200.f, 200.f, 10.f);
	InCard->PopupCard(CardLocation, DesiredLocation, PopupDuration);
	// 0.8s内，禁止抓取卡牌
	auto AllCard = InContextSystem->GetUsingCards();
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

void UProjectJCardExecuteHelper::IntervalScaleSpawnNewCard(AProjectJCardBase* InCard)
{
}

void UProjectJCardExecuteHelper::DestroyCard(int32 InCardID)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->RecycleByID(InCardID);
}
