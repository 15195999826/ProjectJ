// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Game/GAS/ProjectJAbilitySystemComponent.h"
#include "ProjectJMontagePostTagAbilityTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJMontagePostTagAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ProjectJ|Ability|Tasks", meta = (DisplayName="Wait MontagePost", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UProjectJMontagePostTagAbilityTask* CreateMontagePost(UGameplayAbility* OwningAbility, FGameplayTag EventTag, bool OnlyTriggerOnce = false);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventReceivedDelegate, FProjectJMontageEventData, Payload);
	
	UPROPERTY(BlueprintAssignable)
	FEventReceivedDelegate EventReceived;

	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate TaskFailed;
	
protected:
	FGameplayTag Tag;
	bool OnlyTriggerOnce = false;
	FDelegateHandle MyHandle;

	void MontagePostCallback(FProjectJMontageEventData* PanguMontageEventData);
};
