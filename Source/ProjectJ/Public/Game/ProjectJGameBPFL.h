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
#include "ProjectJGameBPFL.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGameBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Equip(AProjectJCharacter* InCharacter, FName InRowName, EProjectJItemType InType);
	
	static void UnEquip();

	inline static FActiveGameplayEffectHandle EmptyActiveGameplayEffectHandle = FActiveGameplayEffectHandle();
	
	static FGameplayEffectSpecHandle SimpleMakeGESpecHandle(AActor* Source, TSubclassOf<UGameplayEffect> GEClass, float Level = 1.0f);
	
	static FActiveGameplayEffectHandle SimpleApplyGEToSelf(AActor* Source, TSubclassOf<UGameplayEffect> GE, float Level = 1.0f);

	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="OutValid"))
	static FProjectJBattleEventData TryGetProjectJGameEventData(const FGameplayEventData& InData, EProjectJValid& OutValid);
};
