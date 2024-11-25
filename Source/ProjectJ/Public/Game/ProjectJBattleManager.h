// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJBattleEventData.h"
#include "ProjectJBattleManager.generated.h"

class AProjectJCharacter;
class AProjectJDebugBattleGameMode;

UENUM()
enum class EProjectJBattleStage : uint8
{
	EnterBattle,
	RoundStart,
	CharacterPrepareAttack, // 无事件，目前用于确定，当前玩家应该执行几次攻击循环
	CharacterStartAttack,
	CharacterDoAttack,
	CharacterEndAttack,
	RoundEnd,

	EndBattle,
};

USTRUCT(BlueprintType)
struct FProjectJExecEventRet
{
	GENERATED_BODY()

	FProjectJExecEventRet()
	{
		
	}
};

USTRUCT()
struct FProjectJBattleContext
{
	GENERATED_BODY()

	FProjectJBattleContext(){}
	
	int32 Round = 0;

	TArray<int32> TopTeam;
	TArray<int32> BottomTeam;
	
	// 行动顺序， 角色ID
	TArray<int32> OrderArray;
	int32 CurrentOrderIndex = 0;
	int32 AttackTimes = 0;
	int32 AttackTotalTimes = 1;
};

UCLASS()
class PROJECTJ_API AProjectJBattleManager : public AActor
{
	GENERATED_BODY()

	friend AProjectJDebugBattleGameMode;
public:	
	// Sets default values for this actor's properties
	AProjectJBattleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartBattle();

	UFUNCTION(BlueprintCallable)
	FProjectJExecEventRet ExecuteEvent(const FProjectJBattleEventData& InEventData);
protected:
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJCharacter>> BattleCharacterMap;

private:
	bool IsRunningBattle = false;
	
	UPROPERTY()
	FProjectJBattleContext BattleContext;

	// --- 战斗状态机相关 Start ---
	inline static TMap<EProjectJBattleStage, FString> BattleStageToStr = {
		{EProjectJBattleStage::EnterBattle, TEXT("EnterBattle")},
		{EProjectJBattleStage::RoundStart, TEXT("RoundStart")},
		{EProjectJBattleStage::CharacterPrepareAttack, TEXT("CharacterPrepareAttack")},
		{EProjectJBattleStage::CharacterStartAttack, TEXT("CharacterStartAttack")},
		{EProjectJBattleStage::CharacterDoAttack, TEXT("CharacterDoAttack")},
		{EProjectJBattleStage::CharacterEndAttack, TEXT("CharacterEndAttack")},
		{EProjectJBattleStage::RoundEnd, TEXT("RoundEnd")},
		{EProjectJBattleStage::EndBattle, TEXT("EndBattle")},
	};

	// Todo: Pending功能
	TArray<FString> WaitSignals;
	EProjectJBattleStage Stage = EProjectJBattleStage::EnterBattle;

	void ChangeStage(EProjectJBattleStage NewStage);
	void OnStageChange(EProjectJBattleStage OldStage, EProjectJBattleStage NewStage);
	
	// --- 战斗状态机相关 End ---

	void RoundStart();

	
};
