// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/ProjectJLevelConfig.h"
#include "ProjectJContextSystem.generated.h"

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

USTRUCT()
struct FProjectJGameContext
{
	GENERATED_BODY()

	FProjectJGameContext(){}

	FName CurrentLevel = NAME_None;

	TArray<int32> LevelCharacters;
	TArray<int32> LevelLandmarks;
	TArray<int32> LevelNavPoints;

	TArray<int32> HandSpellCards;

	// 用于计算是否首次进入某个关卡
	TArray<FName> HasEnteredLevels;
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
	inline static FVector HiddenLocation = FVector(0, 0, -10000);
	UPROPERTY()
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

	UPROPERTY()
	TWeakObjectPtr<AProjectJCardBase> WeakFocusCard;
	
	// 简单的CardPool
public:
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJSpell>> UsingSpells;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJCharacter>> UsingCharacters;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJLandmark>> UsingLandmarks;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJLandmark>> UsingItems;
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AProjectJNavPointActor>> UsingNavPoints;
	
	TArray<TObjectPtr<AProjectJCardBase>> GetUsingCards();
	
	
	AProjectJSpell* CreateSpell(const FName& Config);
	AProjectJCharacter* CreateCharacter(const FName& Config);
	AProjectJLandmark* CreateLandMark(const FName& Config);
	AProjectJNavPointActor* CreateNavPoint(const FProjectJNavPoint& Config);

	void RecycleByID(int32 ID);
	void RecycleSpell(AProjectJSpell* Spell);
	void RecycleCharacter(AProjectJCharacter* Character);
	void RecycleLandMark(AProjectJLandmark* LandMark);
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
	TArray<AProjectJNavPointActor*> NavPointPool;

	AProjectJSpell* GetSpell();
	AProjectJCharacter* GetCharacter();
	AProjectJLandmark* GetLandMark();
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
};
