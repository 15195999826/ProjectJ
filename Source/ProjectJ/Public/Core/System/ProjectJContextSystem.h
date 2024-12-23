// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/ProjectJDateTime.h"
#include "Types/ProjectJLevelConfig.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJContextSystem.generated.h"

class AProjectJUtility;
class AProjectJItem;
class AProjectJSpellArea;
class AProjectJEffectActor;
class AProjectJBattleManager;
class AProjectJPerformManager;
class AProjectJLuaExecutor;
class AProjectJSceneUIManager;
class AProjectJCardBase;
class AProjectJLevelSettingActor;
class AProjectJNavPointActor;
class AProjectJGameProgress;
class AProjectJLandmark;
class AProjectJCharacter;
class AProjectJSpell;

// USTRUCT()
// struct FProjectJGameContext
// {
// 	GENERATED_BODY()
//
// 	FProjectJGameContext(){}
//
// 	UPROPERTY()
// 	FName CurrentLevel = NAME_None;
// 	UPROPERTY()
// 	FProjectJDateTime DateTime;
// 	UPROPERTY()
// 	TArray<int32> LevelCharacters;
// 	UPROPERTY()
// 	TArray<int32> LevelLandmarks;
// 	UPROPERTY()
// 	TArray<int32> LevelNavPoints;
// 	UPROPERTY()
// 	TArray<int32> HandSpellCards;
//
// 	// 用于计算是否首次进入某个关卡
// 	TArray<FName> HasEnteredLevels;
//
// 	
};

USTRUCT()
struct FProjectJEffectContainer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AProjectJEffectActor*> Pool;
};

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJContextSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	inline static FVector HiddenLocation = FVector(-5000, -5000, -10000);
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AProjectJLevelSettingActor> LevelSettingActor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AProjectJSceneUIManager> SceneUIManager;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AProjectJGameProgress> GameProgress;
	UPROPERTY()
	TObjectPtr<AProjectJPerformManager> PerformManager;
	UPROPERTY()
	TObjectPtr<AProjectJLuaExecutor> LuaExecutor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AProjectJBattleManager> BattleManager;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AProjectJSpellArea> SpellArea;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AProjectJCardExecuteArea> ExecuteArea;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AProjectJCardLayoutManager> CardLayoutManager;
	
	// RuntimeData
public:
	UPROPERTY()
	FProjectJGameContext GameContext;

	// UPROPERTY()
	// TWeakObjectPtr<AProjectJCardBase> WeakFocusCard;
	
	// 简单的CardPool
public:
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJSpell>> UsingSpells;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJCharacter>> UsingCharacters;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJLandmark>> UsingLandmarks;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJUtility>> UsingUtilities;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJItem>> UsingItems;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJNavPointActor>> UsingNavPoints;
	
	TArray<TObjectPtr<AProjectJCardBase>> GetUsingCards();
	TMap<int32, TObjectPtr<AProjectJCardBase>> GetUsingCardsMap();
	
	AProjectJSpell* CreateSpell(const FName& Config);
	AProjectJCharacter* CreateCharacter(const FName& Config);
	AProjectJLandmark* CreateLandMark(const FName& Config);
	AProjectJUtility* CreateUtility(const FName& Config);
	AProjectJItem* CreateItem(const FName& Config, EProjectJItemType InType);

	AProjectJNavPointActor* CreateNavPoint(const FProjectJNavPoint& Config);
	
	void RecycleByID(int32 ID);
	void RecycleSpell(AProjectJSpell* Spell);
	void RecycleCharacter(AProjectJCharacter* Character);
	void RecycleLandMark(AProjectJLandmark* LandMark);
	void RecycleUtility(AProjectJUtility* Utility);
	void RecycleItem(AProjectJItem* Item);
	void RecycleNavPoint(AProjectJNavPointActor* NavPoint);
protected:
	int32 GID = 0;
	
	UPROPERTY()
	TArray<AProjectJSpell*> SpellPool;
	UPROPERTY()
	TArray<AProjectJCharacter*> CharacterPool;
	UPROPERTY()
	TArray<AProjectJLandmark*> LandmarkPool;
	UPROPERTY()
	TArray<AProjectJUtility*> UtilityPool;
	UPROPERTY()
	TArray<AProjectJItem*> ItemPool;
	UPROPERTY()
	TArray<AProjectJNavPointActor*> NavPointPool;

	AProjectJSpell* GetSpell();
	AProjectJCharacter* GetCharacter();
	AProjectJLandmark* GetLandMark();
	AProjectJUtility* GetUtility();
	AProjectJItem* GetItem();
	AProjectJNavPointActor* GetNavPoint();
	
	void GeneralOnGet(AActor* InActor);
	void GeneralOnRecycle(AActor* InActor);

	// ----- 特效池子 Start-----
public:
	AProjectJEffectActor* GetEffectActor(TSubclassOf<UObject> InEffectClass);
	void RecycleEffectActor(AProjectJEffectActor* InEffectActor);

protected:
	UPROPERTY()
	TMap<FName,FProjectJEffectContainer> EffectActorPool;
	// ----- 特效池子 End-----


	// ----- 辅助函数 Start-----
public:
	bool IsOverlapExecuteArea(const FVector& InCardPosition) const;
	bool IsInExecuteArea(const FVector& InCardPosition) const;
	void StepLogicFrameCount()
	{
		LogicFrameCount++;
	}
	int32 GetLogicFrameCount() const
	{
		return LogicFrameCount;
	}

private:
	int32 LogicFrameCount = 0;
};
