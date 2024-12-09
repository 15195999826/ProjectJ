// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnLuaInterface.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJBattleEventData.h"
#include "Types/ProjectJLuaInstanceType.h"
#include "ProjectJLuaExecutor.generated.h"

UCLASS()
class PROJECTJ_API AProjectJLuaExecutor : public AActor, public IUnLuaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLuaExecutor();

	virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("ProjectJLuaExecutor");
	}

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void FirstTimeEnterLevel(const FName& RowName, const FName& ScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void EnterLevel(const FName& RowName);
	
protected:
	// UFUNCTION(BlueprintImplementableEvent)


	// ----- 战斗技能相关 Start -----
public:
	UFUNCTION(BlueprintImplementableEvent)
	void CreateLuaAbilityInstance(int32 InCharacterID, int32 InEventID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	FGameplayTag GetExecTag(int32 OwnerID, int32 EventID);
	/**
	 * 获取特性标签
	 * @param OwnerID
	 * @param EventID
	 * @return Key:FeatureTag, Value:Count
	 */
	UFUNCTION(BlueprintImplementableEvent)
	TMap<FGameplayTag, int32> GetFeatureTag(int32 OwnerID, int32 EventID);

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteLuaAbility(int32 InOwnerID, int32 EventID, const FProjectJBattleEventData& AutoDwEventData);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveLuaAbilityInstance(int32 OwnerID, int32 EventID);
	// ----- 战斗技能相关 End -----
};
