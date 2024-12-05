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
	AfterDoAttack,
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

	/**
	 * 位置数据
	 * StartAttack时， 对应卡牌旋转朝向的位置
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationPayload;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatPayload = -1;

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

	// Todo: 变量重命名
	// ----- 这部分其实都是Knock动画的设置 Start -----
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
	

	// 表演层， 当卡牌攻击命中后， 动画状态切换到了AfterDoAttack， 此时在Tick中，卡牌会下落至地面和重置旋转
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Program|Konck")
	float OnAttackHitDropDownSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Program|Konck")
	float OnAttackHitDropDownRotateSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Program|Konck", meta=(DisplayName="(后摇)撞击完毕后多久切换至AfterAttack动画状态"))
	float KnockBackSwing = 0.5f;
	
	// ----- 这部分其实都是Knock动画的设置 end -----
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dead, meta=(DisplayName="死亡后开始溶解延迟"))
	float StartDissolveDelay = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Dead, meta=(DisplayName="溶解持续时间"))
	float DissolveDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="重整队伍")
	float TeamFillDuration = 1.5f;
	
};
