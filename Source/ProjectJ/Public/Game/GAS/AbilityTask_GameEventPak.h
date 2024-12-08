// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GameEventPak.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectJWaitGameplayEventDelegate, FGameplayEventData, Payload);
/**
 * 
 */
UCLASS()
class PROJECTJ_API UAbilityTask_GameEventPak : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GameEventPak* ProjectJGameEventPak(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FProjectJWaitGameplayEventDelegate EventReceived;
	
protected:
	TMap<FGameplayTag, FDelegateHandle> GameEventHandleMap;
	
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void GameplayEventCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload) const;
};
