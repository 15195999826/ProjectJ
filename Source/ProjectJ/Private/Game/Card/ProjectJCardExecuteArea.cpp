// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Components/BoxComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJGameBFL.h"
#include "Game/ProjectJGameContext.h"
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
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnSelectTarget.AddUObject(this, &AProjectJCardExecuteArea::OnSelectTarget);
}

void AProjectJCardExecuteArea::CustomTick(int32 InLogicFrameCount)
{
	// 控制卡牌隐藏在Tick返回执行完毕的下一帧执行
	if (DuringHiding && !StartHidingNextTick)
	{
		StartHidingNextTick = true;
		const auto CardType = IProjectJCardInterface::Execute_GetCardType(ExecutingCard.Get());
		// 执行结束, 播放卡牌隐藏动画
		ExecutingCard->HideCard(0.5f);
		// 等待动画结束后
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, CardType]()
		{
			int32 LocalID = ExecutingCard->ID;
			ExecutingCard = nullptr;
			// Todo: 装备卡的功能特殊实现, 执行后，装备到角色身上
			if (CachedItemSecondaryType == EProjectJItemType::None || CachedItemSecondaryType ==
				EProjectJItemType::Prop)
			{
				auto LocalContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
				LocalContextSystem->LuaExecutor->ExecuteAfterHide(CardType, LocalID);
			}
		}, 0.5f, false);
	}
	
	if (ExecutingCard.IsValid() && !DuringHiding)
	{
		auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
		// 时间流逝, 每一帧 + 1 分钟
		ContextSystem->GameContext->DateTime.StepMinute();
		
		const auto CardType = IProjectJCardInterface::Execute_GetCardType(ExecutingCard.Get());
		// Todo: 对话怎么形式
		ContextSystem->LuaExecutor->ExecuteTick(CardType, ExecutingCard->ID, InLogicFrameCount);

		PostExecutePercent((InLogicFrameCount - StartFrame) / (CachedMinutes * 10.f));
		
		if (InLogicFrameCount - StartFrame >= CachedMinutes * 10)
        {
			ContextSystem->LuaExecutor->ExecuteOver(CardType, ExecutingCard->ID);
            DuringHiding = true;;
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
		case EProjectJCardType::Spell:
		case EProjectJCardType::Utility:
			{
				const auto& TargetFilter = ContextSystem->LuaExecutor->GetTargetFilter(InCard->ID, CardType);
				// 高亮满足条件的卡牌
				auto AllCards = ContextSystem->GameContext->InDungeonCardsMap;
				for (auto& Pair : AllCards)
				{
					if (Pair.Key == InCard->ID)
					{
						continue;
					}
					
					if (SatisfyFilter(Pair.Value.Get(), TargetFilter))
					{
						Pair.Value->OnWaitForExecuteSelect();
					}
				}

				ShowTips(NSLOCTEXT("ProjectJ", "SelectTarget", "请选择目标"));
				// 等待选择目标
				auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
				EventSystem->WaitForSelectTarget.Broadcast();
			}
			break;
	}

	// Todo: 待讨论，是否需要Tick功能
	CachedMinutes =ContextSystem->LuaExecutor->GetExecuteMinutes(CardType, InCard->ID);
	StartFrame = ContextSystem->GetLogicFrameCount();
	ContextSystem->LuaExecutor->ExecuteStart(CardType, InCard->ID, StartFrame);
	StartHidingNextTick = false;
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

void AProjectJCardExecuteArea::OnSelectTarget(AProjectJCardBase* InCard)
{
	HideTips();
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	for (auto& Pair : ContextSystem->GameContext->InDungeonCardsMap)
	{
		Pair.Value->EndExecuteSelect();
	}
	InCard->CanDrag = false;
	InCard->PerformSelected();
	SelectedCard = InCard;
	auto CardType = IProjectJCardInterface::Execute_GetCardType(ExecutingCard.Get());
	auto SelectedCardType = IProjectJCardInterface::Execute_GetCardType(InCard);
	ContextSystem->LuaExecutor->ExecuteSelectTarget(CardType, ExecutingCard->ID, SelectedCardType, InCard->ID);
}

void AProjectJCardExecuteArea::OnLoseFocus_Implementation()
{
}

void AProjectJCardExecuteArea::OnFocus_Implementation()
{
}
