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
	AfterAttackHit,
	OnBeHit,
	Death
};

USTRUCT(BlueprintType)
struct FProjectJCharacterAniData
{
	GENERATED_BODY()
	
	FProjectJCharacterAniData()
	{
	}

	FProjectJCharacterAniData(bool IsEmpty): IsEmpty(IsEmpty)
	{
	}
	
	bool IsEmpty = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AProjectJCharacter>> TargetCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationPayload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> AnimationKVs;

	static const FProjectJCharacterAniData Empty;
};

USTRUCT(BlueprintType)
struct FProjectJProgramAnimationSettings
{
	GENERATED_BODY()
	
	FProjectJProgramAnimationSettings()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=AttackStart)
	float AttackStartDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=AttackStart)
	FVector AttackStartTargetLocationOffset = FVector(-100, 0, 100);

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=AttackStart)
	float AttackStartTargetPitch = -10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=DoAttack)
	float DoAttackDuration = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=DoAttack)
	FVector DoAttackTargetLocationOffset = FVector(-150, 0, 0);
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=DoAttack)
	float DoAttackMoveBackWorldOffset = -50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=DoAttack)
	float DoAttackMoveBackWorldPitch = -15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category=DoAttack)
	float DoAttackPlayMontagePercent = 0.3f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=OnAttackHit)
	float OnAttackHitDropDownSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=OnAttackHit)
	float OnAttackHitDropDownRotateSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=OnAttackHit)
	float OnAttackHitDelayToNextStage = 0.5f;
};
