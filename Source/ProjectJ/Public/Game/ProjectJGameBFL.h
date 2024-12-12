// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Card/ProjectJCharacter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/ProjectJBattleEventData.h"
#include "Types/Item/ProjectJItemBase.h"
#include "Types/ProjectJValid.h"
#include "ProjectJGameBFL.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGameBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Equip(AProjectJCharacter* InCharacter, FName InRowName, EProjectJItemType InType);
	
	static void UnEquip();

	inline static FActiveGameplayEffectHandle EmptyActiveGameplayEffectHandle = FActiveGameplayEffectHandle();
	
	static FGameplayEffectSpecHandle SimpleMakeGESpecHandle(AActor* Source, TSubclassOf<UGameplayEffect> GEClass, float Level = 1.0f);

	// UFUNCTION(BlueprintCallable, Category="Spawning", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true", DeprecatedFunction, DeprecationMessage="Use BeginSpawningActorFromClass"))
	// static ENGINE_API class AActor* BeginSpawningActorFromBlueprint(const UObject* WorldContextObject, const class UBlueprint* Blueprint, const FTransform& SpawnTransform, bool bNoCollisionFail);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static FText GetLuaAbilityDesc(const UObject* WorldContextObject, const FName& InLuaScriptCHSName);

	UFUNCTION(BlueprintCallable)
	static FActiveGameplayEffectHandle SimpleApplyGEToSelf(AActor* Source, TSubclassOf<UGameplayEffect> GE, float Level = 1.0f, int32 Round = -1);

	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="OutValid"))
	static FProjectJBattleEventData TryGetProjectJGameEventData(const FGameplayEventData& InData, EProjectJValid& OutValid);
};
