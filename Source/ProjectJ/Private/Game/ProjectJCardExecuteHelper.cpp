// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardExecuteHelper.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJItem.h"
#include "Game/Card/ProjectJLandmark.h"

int32 UProjectJCardExecuteHelper::SpawnNewCard(const FName& InRowName, EProjectJCardType InType,
                                               EProjectJItemType InItemType)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnNewCard, RowName: %s, CardType: %d, ItemType: %d"), *InRowName.ToString(), (int32)InType, (int32)InItemType);
	
	UProjectJContextSystem* ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto ExecAreaLocation = ContextSystem->ExecuteArea->GetActorLocation();
	int32 RetID = -1;
	AProjectJCardBase* NewCard;
	switch (InType) {
		case EProjectJCardType::Character:
			{
				NewCard = ContextSystem->CreateCharacter(InRowName);
			}
			break;
		case EProjectJCardType::Landmark:
			{
				NewCard = ContextSystem->CreateLandMark(InRowName);
			}
			break;
		case EProjectJCardType::Item:
			{
				NewCard = ContextSystem->CreateItem(InRowName, InItemType);
			}
			break;
		default:
			{
				UE_LOG(LogTemp, Error, TEXT("不支持生成的 CardType %d"), (int32)InType);
				return RetID;
			}
	}
	RetID = NewCard->ID;
	NewCard->SetActorLocation(ExecAreaLocation);
	float PopupDuration = 0.8f;
	auto DesiredLocation = ExecAreaLocation + FVector(200.f, 200.f, 10.f);
	NewCard->PopupCard(ExecAreaLocation, DesiredLocation, PopupDuration);
	// 0.8s内，禁止抓取卡牌
	auto AllCard = ContextSystem->GetUsingCards();
	for (auto& Card : AllCard)
	{
		Card->CanDrag = false;
	}
	FTimerHandle UnlockTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(UnlockTimerHandle, [NewCard, ContextSystem, AllCard, DesiredLocation]() {
		auto GroundDesiredLocation = DesiredLocation;
		GroundDesiredLocation.Z = 0.f;
		ContextSystem->CardLayoutManager->PlaceCardAndRelayout(NewCard, 0.1f, GroundDesiredLocation);
		for (auto& Card : AllCard)
		{
			Card->CanDrag = true;
		}
	}, PopupDuration - 0.033f, false);
	
	return RetID;
}

void UProjectJCardExecuteHelper::DestroyCard(int32 InCardID)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->RecycleByID(InCardID);
}
