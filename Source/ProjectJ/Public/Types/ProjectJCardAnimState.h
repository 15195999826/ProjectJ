#pragma once

#include "CoreMinimal.h"

#include "ProjectJCardAnimState.generated.h"

class AProjectJCharacter;

UENUM(BlueprintType)
enum class EProjectJCardAnimState : uint8
{
	Idle,
	AttackStart,
	WaitingAttack,
	DoAttack,
	OnAttackHit,
	Death
};

USTRUCT(BlueprintType)
struct FProjectJCharacterAniData
{
	GENERATED_BODY()
	
	FProjectJCharacterAniData()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AProjectJCharacter>> TargetCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationPayload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> AnimationKVs;

	static const FProjectJCharacterAniData Empty;
};
