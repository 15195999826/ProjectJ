#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

#include "ProjectJBattleEventData.generated.h"

USTRUCT(BlueprintType)
struct PROJECTJ_API FProjectJBattleEventData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	FProjectJBattleEventData(): ExecutorID(0), TriggeredID(0), TargetID(0)
	{
	}

	FProjectJBattleEventData(const FGameplayTag& InTag): EventTag(InTag), ExecutorID(0), TriggeredID(0), TargetID(0)
	{
	}
	
	FProjectJBattleEventData(const FGameplayTag& InTag, int32 InExectorID, int32 InTriggeredID, int32 InTargetID): EventTag(InTag), ExecutorID(InExectorID),
																		  TriggeredID(InTriggeredID), TargetID(InTargetID)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EventTag;

	// 举例： A死亡，A是TriggeredID;B的功能是：队友昏厥是,XXX， ExecutorID是B;TargetID是，B的效果的执行目标
	// 谁在执行本次事件
	UPROPERTY(BlueprintReadOnly)
	int32 ExecutorID;

	// 谁触发了本次事件
	UPROPERTY(BlueprintReadOnly)
	int32 TriggeredID;

	// 本次事件的执行目标
	UPROPERTY(BlueprintReadOnly)
	int32 TargetID;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FString> EventKVs;
	
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FProjectJBattleEventData::StaticStruct();
	}
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FProjectJBattleEventData> : public TStructOpsTypeTraitsBase2<FProjectJBattleEventData>
{
	enum
	{
		WithNetSerializer = true
	};
};