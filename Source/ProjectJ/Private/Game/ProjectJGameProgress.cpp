// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameProgress.h"

#include "Game/ProjectJGameMode.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJLevelSettingActor.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/ProjectJNavPointActor.h"
#include "Game/ProjectJPerformManager.h"
#include "Game/ProjectJSceneUIManager.h"
#include "Game/ProjectJSpellArea.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJLandmark.h"
#include "Game/Card/ProjectJSpell.h"
#include "Types/ProjectJLevelConfig.h"

// Sets default values
AProjectJGameProgress::AProjectJGameProgress()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectJGameProgress::BeginPlay()
{
	Super::BeginPlay();
	auto GM = GetWorld()->GetAuthGameMode<AProjectJGameMode>();
	GM->OnLevelPrepared.AddUObject(this, &AProjectJGameProgress::OnLevelPrepared);

	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnPerformEnd.AddUObject(this, &AProjectJGameProgress::OnPerformEnd);
	
	EventSystem->OnObserveCard.AddUObject(this, &AProjectJGameProgress::OnObserveCard);
	EventSystem->OnCheckCard.AddUObject(this, &AProjectJGameProgress::OnCheckCard);
	EventSystem->OnHideAtCard.AddUObject(this, &AProjectJGameProgress::OnHideAtCard);
	EventSystem->OnAmbushCard.AddUObject(this, &AProjectJGameProgress::OnAmbushCard);

	DeltaTimeAccumulator = 0.f;
}

// Called every frame
void AProjectJGameProgress::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTimeAccumulator += DeltaTime;
	// 更新逻辑帧数 逻辑帧率按60帧计算
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	if (DeltaTimeAccumulator >= 1.f / 60.f)
	{
		DeltaTimeAccumulator -= 1.f / 60.f;
		ContextSystem->StepLogicFrameCount();
		ContextSystem->ExecuteArea->CustomTick(ContextSystem->GetLogicFrameCount());
	}

	switch (GameStage) {
		case EProjectJGameStage::None:
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameStage is None, Error"));
			}
			break;
		case EProjectJGameStage::Idle:
			break;
		default:
			{
				bool CompleteStage = true;
				for (auto& Pair : WaitingSignals)
				{
					if (Pair.Value == false)
					{
						CompleteStage = false;
						break;
					}
				}

				if (CompleteStage)
				{
					WaitingSignals.Empty();
					ChangeStage(EProjectJGameStage::Idle, EmptyPayload);
				}
			}
			break;
	}
}

void AProjectJGameProgress::OnLevelPrepared()
{
}

void AProjectJGameProgress::SpellCardToArea(AProjectJCardBase* InCard, int32 Index, const FVector& CenterLocation,
	const FVector& Offset)
{
	// 总共4张牌， 居中摆放, 1和2在中间，0和3在两侧
	FVector NewLocation = CenterLocation + Offset * (Index - 1.5f);
	InCard->SetActorLocation(NewLocation);
}

void AProjectJGameProgress::OnLeaveStage(EProjectJGameStage OldStage, FProjectJChangeStagePayload Payload)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	switch (OldStage) {
		case EProjectJGameStage::None:
			break;
		case EProjectJGameStage::Idle:
			break;
		case EProjectJGameStage::Observing:
			{
				// 观察卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[0]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToArea(Card.Get(), 0, ContextSystem->SpellArea->GetActorLocation(), LevelSettingActor->HandSpellCardOffset);
			}
			break;
		case EProjectJGameStage::Checking:
			{
				// 检查卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[1]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToArea(Card.Get(), 1, ContextSystem->SpellArea->GetActorLocation(), LevelSettingActor->HandSpellCardOffset);
			}
			break;
		case EProjectJGameStage::Hiding:
			{
				// 隐藏卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[2]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToArea(Card.Get(), 2, ContextSystem->SpellArea->GetActorLocation(), LevelSettingActor->HandSpellCardOffset);
			}
			break;
		case EProjectJGameStage::Ambushing:
			{
				// 偷袭卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[3]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToArea(Card.Get(), 3, ContextSystem->SpellArea->GetActorLocation(), LevelSettingActor->HandSpellCardOffset);
			}
			break;
	}
}

void AProjectJGameProgress::StartNewGame()
{
	// 生成4张Spell, 观察、检查、躲藏、偷袭
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->GameContext = FDecoraptedProjectJGameContext();
	auto Spell1 = ContextSystem->CreateSpell(TEXT("观察"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell1->ID);
	auto Spell2 = ContextSystem->CreateSpell(TEXT("检查"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell2->ID);
	auto Spell3 = ContextSystem->CreateSpell(TEXT("躲藏"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell3->ID);
	auto Spell4 = ContextSystem->CreateSpell(TEXT("偷袭"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell4->ID);
	// 放到手牌位置, Attach To MainCamera
	auto LevelSettingActor = ContextSystem->LevelSettingActor;
	auto HandSpellCardStartLocation = ContextSystem->SpellArea->GetActorLocation();
	auto HandSpellCardOffset = LevelSettingActor->HandSpellCardOffset;
	SpellCardToArea(Spell1, 0, HandSpellCardStartLocation, HandSpellCardOffset);
	SpellCardToArea(Spell2, 1, HandSpellCardStartLocation, HandSpellCardOffset);
	SpellCardToArea(Spell3, 2, HandSpellCardStartLocation, HandSpellCardOffset);
	SpellCardToArea(Spell4, 3, HandSpellCardStartLocation, HandSpellCardOffset);
	
	EnterLevel(StartLevel.RowName);
}

void AProjectJGameProgress::EnterLevel(const FName& LevelRowName)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	// 移除当前关卡中元素
	for (auto& LandmarkID : ContextSystem->GameContext.LevelLandmarks)
	{
		ContextSystem->RecycleByID(LandmarkID);
	}
	for (auto& CharacterID : ContextSystem->GameContext.LevelCharacters)
	{
		ContextSystem->RecycleByID(CharacterID);
	}

	for (auto& NavPointID : ContextSystem->GameContext.LevelNavPoints)
	{
		ContextSystem->RecycleByID(NavPointID);
	}
	
	// 加载关卡
	ContextSystem->GameContext.CurrentLevel = LevelRowName;
	auto LevelConfig = GetDefault<UProjectJDataTableSettings>()->LevelTable.LoadSynchronous()->FindRow<FProjectJLevelConfig>(LevelRowName, TEXT(""));
	check(LevelConfig);
	
	// 生成Landmark
	for (auto& Landmark : LevelConfig->Landmarks)
	{
		auto LandmarkActor = ContextSystem->CreateLandMark(Landmark.RowName);
		LandmarkActor->SetActorLocation(Landmark.Location);
		LandmarkActor->CanDrag = true;
		ContextSystem->GameContext.LevelLandmarks.Add(LandmarkActor->ID);
	}
	
	// 生成Character
	for (auto& Character : LevelConfig->Characters)
	{
		auto Char = ContextSystem->CreateCharacter(Character.RowName);
		Char->SetActorLocation(Character.Location);
		Char->CanDrag = true;
		ContextSystem->GameContext.LevelCharacters.Add(Char->ID);
	}
	// 生成NavPoint
	for (auto& NavPoint : LevelConfig->NavPoints)
	{
		auto NavPointActor = ContextSystem->CreateNavPoint(NavPoint);
		NavPointActor->CanDrag = false;
		ContextSystem->GameContext.LevelNavPoints.Add(NavPointActor->ID);
	}

	// 首次进入关卡
	if (!ContextSystem->GameContext.HasEnteredLevels.Contains(LevelRowName))
	{
		ContextSystem->GameContext.HasEnteredLevels.Add(LevelRowName);
		ContextSystem->LuaExecutor->FirstTimeEnterLevel(LevelRowName, LevelConfig->LuaScriptName);
	}

	ContextSystem->LuaExecutor->EnterLevel(LevelRowName);
	
	ChangeStage(EProjectJGameStage::Performing, EmptyPayload);
}

void AProjectJGameProgress::ChangeStage(EProjectJGameStage NewStage, const FProjectJChangeStagePayload& Payload)
{
	if (NewStage == GameStage)
	{
		UE_LOG(LogTemp, Warning, TEXT("相同状态切换，逻辑可能错误, Stage: %d"), (int32)NewStage);
		return;
	}

	if (WaitingSignals.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("上一个阶段尚未完成, Stage: %d"), (int32)GameStage);
		return;
	}
	
	auto OldStage = GameStage;
	GameStage = NewStage;
	OnLeaveStage(OldStage, Payload);
	OnEnterStage(Payload);
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnStageChange.Broadcast(OldStage, NewStage);
}

void AProjectJGameProgress::OnEnterStage(const FProjectJChangeStagePayload& Payload)
{
	switch (GameStage) {
		case EProjectJGameStage::None:
			break;
		case EProjectJGameStage::Idle:
			{
				
			}
			break;
		case EProjectJGameStage::Observing:
			{
				WaitingSignals.Add(SignalObserving, false);
				auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
				ContextSystem->SceneUIManager->OnTalkOver.AddDynamic(this, &AProjectJGameProgress::OnTalkOver);
				ContextSystem->SceneUIManager->StartTalkWith(Payload.CardPayload.Get());
			}
			break;
		case EProjectJGameStage::Checking:
		case EProjectJGameStage::Hiding:
		case EProjectJGameStage::Ambushing:
			{
				// Todo: 尚未处理的功能
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("尚未处理的功能"));
			}
			break;
		case EProjectJGameStage::Performing:
			{
				WaitingSignals.Add(SignalPerforming, false);
				auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
				ContextSystem->PerformManager->StartPerform();
			}
			break;
	}
}

void AProjectJGameProgress::OnPerformEnd()
{
	check(GameStage == EProjectJGameStage::Performing);
	check(WaitingSignals.Contains(SignalPerforming));
	WaitingSignals[SignalPerforming] = true;
}

void AProjectJGameProgress::OnObserveCard(AProjectJCardBase* InCard)
{
	FProjectJChangeStagePayload Payload;
	Payload.CardPayload = InCard;
	ChangeStage(EProjectJGameStage::Observing, Payload);
}

void AProjectJGameProgress::OnCheckCard(AProjectJCardBase* InCard)
{
	FProjectJChangeStagePayload Payload;
	Payload.CardPayload = InCard;
	ChangeStage(EProjectJGameStage::Checking, Payload);
}

void AProjectJGameProgress::OnHideAtCard(AProjectJCardBase* InCard)
{
	FProjectJChangeStagePayload Payload;
	Payload.CardPayload = InCard;
	ChangeStage(EProjectJGameStage::Hiding, Payload);
}

void AProjectJGameProgress::OnAmbushCard(AProjectJCardBase* InCard)
{
	FProjectJChangeStagePayload Payload;
	Payload.CardPayload = InCard;
	ChangeStage(EProjectJGameStage::Ambushing, Payload);
}

void AProjectJGameProgress::OnTalkOver()
{
	WaitingSignals[SignalObserving] = true;
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->SceneUIManager->OnTalkOver.RemoveDynamic(this, &AProjectJGameProgress::OnTalkOver);
}

