// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJBattleEventData.h"
#include "Types/ProjectJBattleStage.h"
#include "ProjectJBattleManager.generated.h"

class AProjectJCharacter;
class AProjectJDebugBattleGameMode;

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
	
	// 攻击相关数据
	int32 AttackTimes = 0;
	int32 AttackTotalTimes = 1;
	TArray<int32> AttackTargets;
};

USTRUCT()
struct FProjectJTeamFillData
{
	GENERATED_BODY()

	FProjectJTeamFillData(): StartTime(0), Duration(0)
	{
	}

	TMap<int32, FVector> TeamMovingFrom;

	TMap<int32, FVector> TeamMovingTo;
	
	float StartTime;
	float Duration;

	FTimerHandle MovingTimerHandle;
};

USTRUCT()
struct FProjectJEventExec
{
	GENERATED_BODY()

	FProjectJEventExec()
	{
		
	}
	
	UPROPERTY()
	TWeakObjectPtr<AProjectJCharacter> Executor;

	UPROPERTY()
	FProjectJBattleEventData ProjectJEventData;
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

	FVector GetTeamPosition(int32 InTeamID, int32 InPosition, int32 InTotalCount);
protected:
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJCharacter>> BattleCharacterMap;
	
private:
	bool IsRunningBattle = false;
	bool IsBattleEnd();

	// -------缓存数据 Start-------
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> HealEffect;

	// -------缓存数据 End-------
	
	UPROPERTY()
	FProjectJBattleContext BattleContext;

	TWeakObjectPtr<AProjectJCharacter> GetCurrentCharacter();

	// --- 战斗状态机相关 Start ---
	inline static TMap<EProjectJBattleStage, FString> BattleStageToStr = {
		{EProjectJBattleStage::EnterBattle, TEXT("EnterBattle")},
		{EProjectJBattleStage::RoundStart, TEXT("RoundStart")},
		{EProjectJBattleStage::CharacterGetTurn, TEXT("CharacterGetTurn")},
		{EProjectJBattleStage::CharacterStartAttack, TEXT("CharacterStartAttack")},
		{EProjectJBattleStage::CharacterDoAttack, TEXT("CharacterDoAttack")},
		{EProjectJBattleStage::CharacterEndAttack, TEXT("CharacterEndAttack")},
		{EProjectJBattleStage::CharacterEndTurn, TEXT("CharacterEndTurn")},
		{EProjectJBattleStage::RoundEnd, TEXT("RoundEnd")},
		{EProjectJBattleStage::EndBattle, TEXT("EndBattle")},
	};
	
	// Todo: Pending功能
	inline static FName SignalSimplePending = TEXT("SimplePending");
	// 广播事件， 执行期间的信号
	inline static FName SignalEventExecPending = TEXT("EventExecPending");
	TArray<FName> WaitSignals;
	EProjectJBattleStage Stage = EProjectJBattleStage::EnterBattle;

	void ChangeStage(EProjectJBattleStage NewStage);
	void OnStageChange(EProjectJBattleStage OldStage, EProjectJBattleStage NewStage);
	void PostRoundStart();

	// --- 战斗状态机相关 End ---

	void RoundStart();

	void OnWaitingAttack(int InCharacterID);
	void OnAttackHit(int InCharacterID);
	void AfterAttackHit(int InCharacterID);
	void BuildTeamResetCharacters(int32 TeamID, const TArray<int32>& InTeam, TMap<int32, FVector>& WriteMoving);
	void OnIdleReturnToPosition(int InCharacterID);

	// --- 位置重置功能 Start ---
	FProjectJTeamFillData TeamFillData;
	void UpdateTeamFill();

	// --- 位置重置功能 End ---


	// --- 词条功能 Start ---
public:
	UFUNCTION()
	FProjectJExecEventRet ExecuteEvent(const FProjectJBattleEventData& InEventData);

	// 挨个抛出EventQueue中的事件， 并且一个一个角色轮流执行
	UFUNCTION()
	void ProcessEventQueue();
	
protected:
	bool IsProcessingEventQueue = false;
	bool IsSomeOneRunningEvent = false;

	void PostLuaAbilityStatus(int InCharacterID, bool IsRunningEvent);
	
	
	UPROPERTY()
	TArray<FProjectJBattleEventData> EventQueue;
	UPROPERTY()
	TArray<FProjectJEventExec> EventExecQueue;

	void RecursiveBroadcastEvent(const FProjectJBattleEventData& InEventData);
	
	void IntervalGetDamage(const FProjectJBattleEventData& InEventData);
	void IntervalGetHeal(const FProjectJBattleEventData& InEventData);
	void IntervalGetFeature(const FProjectJBattleEventData& InEventData);
	
	// --- 词条功能 End ---
	

	// --- 外部获取数据 Start---
public:
	int32 GetRound () const { return BattleContext.Round; }

	// --- 外部获取数据 End---


	// --- Only For Lua Start ---
	UFUNCTION(BlueprintCallable)
	bool IsCharacterDead(int32 InCharacterID);

	/**
	 * 获取对手队伍, 只返回存活的
	 * @param InCharacterID
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetOpponentTeam(int32 InCharacterID);

	/**
	 * 获取队友队伍, 只返回存活的
	 * @param InCharacterID
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetTeammateTeam(int32 InCharacterID);
	
	// --- Only For Lua End ---
};
