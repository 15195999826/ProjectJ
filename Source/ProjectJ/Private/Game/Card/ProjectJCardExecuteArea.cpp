// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Components/BoxComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameBFL.h"
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

void AProjectJCardExecuteArea::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectJCardExecuteArea::CustomTick(int32 InLogicFrameCount)
{
	if (ExecutingCard.IsValid() && !DuringHiding)
	{
		auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
		// 时间流逝, 每一帧 + 1 分钟
		ContextSystem->GameContext.DateTime.StepMinute();
		
		
		bool bIsExecuteEnd = false;
		const auto CardType = IProjectJCardInterface::Execute_GetCardType(ExecutingCard.Get());
		
		switch (CardType) {
			case EProjectJCardType::None:
				break;
			case EProjectJCardType::Spell:
				break;
			case EProjectJCardType::Character:
			case EProjectJCardType::Utility:
			case EProjectJCardType::Landmark:
				{
					bIsExecuteEnd = ContextSystem->LuaExecutor->ExecuteTick(CardType, ExecutingCard->ID, InLogicFrameCount);
				}
				break;
			case EProjectJCardType::Item:
				break;
		}

		if (bIsExecuteEnd)
		{
			DuringHiding = true;
			// 执行结束, 播放卡牌隐藏动画
			ExecutingCard->HideCard(0.5f);
			// 等待动画结束后
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, CardType]()
			{
				// Todo: 装备卡的功能特殊实现, 执行后，装备到角色身上
				if (CachedItemSecondaryType == EProjectJItemType::None || CachedItemSecondaryType ==
					EProjectJItemType::Prop)
				{
					auto LocalContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
					LocalContextSystem->LuaExecutor->ExecuteAfterHide(CardType, ExecutingCard->ID);
				}
			
				ExecutingCard = nullptr;
			}, 0.5f, false);
		}
	}
}

void AProjectJCardExecuteArea::StartExecute(AProjectJCardBase* InCard)
{
	CachedItemSecondaryType = EProjectJItemType::None;
	DuringHiding = false;
	ExecutingCard = InCard;

	// Todo: 执行时间流逝

	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	const auto CardType = IProjectJCardInterface::Execute_GetCardType(InCard);
	switch (CardType) {
		case EProjectJCardType::None:
			UKismetSystemLibrary::PrintString(this, "CardType is None", true, true, FLinearColor::Red, 5.f);
			break;
		case EProjectJCardType::Character:
			{
				
			}
			break;
		case EProjectJCardType::Landmark:
			{
				
			}
			break;
		case EProjectJCardType::Item:
			{
				auto Item = Cast<AProjectJItem>(InCard);
				CachedItemSecondaryType = Item->ItemType;
				switch (Item->ItemType) {
					case EProjectJItemType::Weapon:
						break;
					case EProjectJItemType::Armor:
						break;
					case EProjectJItemType::Prop:
						{
							
						}
						break;
					default:
						break;
				}
			}
			break;
		case EProjectJCardType::Utility:
			{
				const auto& TargetFilter = ContextSystem->LuaExecutor->GetTargetFilter(InCard->ID);
				// 高亮满足条件的卡牌
				auto AllCards = ContextSystem->GetUsingCardsMap();
				for (auto& Pair : AllCards)
				{
					if (SatisfyFilter(Pair.Value.Get(), TargetFilter))
					{
						Pair.Value->OnWaitForExecuteSelect();
					}
				}
			}
			break;
		case EProjectJCardType::Spell:
			break;
	}

	ContextSystem->LuaExecutor->ExecuteStart(CardType, InCard->ID, ContextSystem->GetLogicFrameCount());
}

bool AProjectJCardExecuteArea::SatisfyFilter(AProjectJCardBase* InCard, const FProjectJTargetFilter& InTargetFilter)
{
	auto CardType = IProjectJCardInterface::Execute_GetCardType(InCard);
	if (InTargetFilter.TargetTypes.Contains(CardType))
	{
		return true;
	}

	return false;
}

void AProjectJCardExecuteArea::OnLoseFocus_Implementation()
{
}

void AProjectJCardExecuteArea::OnFocus_Implementation()
{
}
