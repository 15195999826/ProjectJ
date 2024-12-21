// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Components/BoxComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectJ/ProjectJLogChannels.h"

AProjectJCardExecuteArea::AProjectJCardExecuteArea()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneRoot);
}

void AProjectJCardExecuteArea::StartExecute(AProjectJCardBase* InCard)
{
	ExecutingCard = InCard;

	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	
	const auto CardType = IProjectJCardInterface::Execute_GetCardType(InCard);
	switch (CardType) {
		case EProjectJCardType::None:
			UKismetSystemLibrary::PrintString(this, "CardType is None", true, true, FLinearColor::Red, 5.f);
			break;
		case EProjectJCardType::Character:
		case EProjectJCardType::Landmark:
			{
				// Todo: 直接执行卡牌功能
			}
			break;
		case EProjectJCardType::Spell:
		case EProjectJCardType::Item:
			{
				// Todo: 选择目标
				ContextSystem->LuaExecutor->GetTargetFilter(InCard);
			}
			break;
	}
}

void AProjectJCardExecuteArea::OnLoseFocus_Implementation()
{
}

void AProjectJCardExecuteArea::OnFocus_Implementation()
{
}
