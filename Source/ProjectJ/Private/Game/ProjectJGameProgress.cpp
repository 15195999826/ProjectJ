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
	
}

// Called every frame
void AProjectJGameProgress::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AProjectJGameProgress::SpellCardToHand(AProjectJCardBase* InCard, int32 Index, const FVector& StartLocation,
	const FVector& Offset, const FRotator& Rotation)
{
	InCard->AttachToActor(GetWorld()->GetFirstPlayerController()->GetViewTarget(), FAttachmentTransformRules::KeepRelativeTransform);
	InCard->SetActorRelativeLocation(StartLocation + Offset * Index);
	InCard->SetActorRelativeRotation(Rotation);
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
				SpellCardToHand(Card.Get(), 0, LevelSettingActor->HandSpellCardStartLocation, LevelSettingActor->HandSpellCardOffset, LevelSettingActor->HandSpellCardRotation);
			}
			break;
		case EProjectJGameStage::Checking:
			{
				// 检查卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[1]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToHand(Card.Get(), 1, LevelSettingActor->HandSpellCardStartLocation, LevelSettingActor->HandSpellCardOffset, LevelSettingActor->HandSpellCardRotation);
			}
			break;
		case EProjectJGameStage::Hiding:
			{
				// 隐藏卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[2]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToHand(Card.Get(), 2, LevelSettingActor->HandSpellCardStartLocation, LevelSettingActor->HandSpellCardOffset, LevelSettingActor->HandSpellCardRotation);
			}
			break;
		case EProjectJGameStage::Ambushing:
			{
				// 偷袭卡牌返回手牌
				auto Card = ContextSystem->UsingSpells[ContextSystem->GameContext.HandSpellCards[3]];
				auto LevelSettingActor = ContextSystem->LevelSettingActor;
				SpellCardToHand(Card.Get(), 3, LevelSettingActor->HandSpellCardStartLocation, LevelSettingActor->HandSpellCardOffset, LevelSettingActor->HandSpellCardRotation);
			}
			break;
	}
}

void AProjectJGameProgress::StartNewGame()
{
	// 生成4张Spell, 观察、检查、躲藏、偷袭
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->GameContext = FProjectJGameContext();
	auto Spell1 = ContextSystem->CreateSpell(TEXT("观察"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell1->ID);
	auto Spell2 = ContextSystem->CreateSpell(TEXT("检查"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell2->ID);
	auto Spell3 = ContextSystem->CreateSpell(TEXT("躲藏"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell3->ID);
	auto Spell4 = ContextSystem->CreateSpell(TEXT("偷袭"));
	ContextSystem->GameContext.HandSpellCards.Add(Spell4->ID);
	// 放到手牌位置, Attach To MainCamera
	auto MainCamera = GetWorld()->GetFirstPlayerController()->GetViewTarget();
	auto LevelSettingActor = ContextSystem->LevelSettingActor;
	auto HandSpellCardStartLocation = LevelSettingActor->HandSpellCardStartLocation;
	auto HandSpellCardOffset = LevelSettingActor->HandSpellCardOffset;
	auto HandSpellCardRotation = LevelSettingActor->HandSpellCardRotation;
	SpellCardToHand(Spell1, 0, HandSpellCardStartLocation, HandSpellCardOffset, HandSpellCardRotation);
	SpellCardToHand(Spell2, 1, HandSpellCardStartLocation, HandSpellCardOffset, HandSpellCardRotation);
	SpellCardToHand(Spell3, 2, HandSpellCardStartLocation, HandSpellCardOffset, HandSpellCardRotation);
	SpellCardToHand(Spell4, 3, HandSpellCardStartLocation, HandSpellCardOffset, HandSpellCardRotation);
	
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
		LandmarkActor->CanDrag = false;
		ContextSystem->GameContext.LevelLandmarks.Add(LandmarkActor->ID);
	}
	
	// 生成Character
	for (auto& Character : LevelConfig->Characters)
	{
		auto Char = ContextSystem->CreateCharacter(Character.RowName);
		Char->SetActorLocation(Character.Location);
		Char->CanDrag = false;
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

