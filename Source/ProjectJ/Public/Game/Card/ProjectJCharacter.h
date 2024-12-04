// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "ProjectJCardBase.h"
#include "Interface/ProjectJBattleInterface.h"
#include "Interface/ProjectJCardInterface.h"
#include "Types/ProjectJCardAnimState.h"
#include "ProjectJCharacter.generated.h"

UENUM()
enum class EProgramAniAttackType
{
	Single,
	
};

USTRUCT()
struct FProgramAttackData
{
	GENERATED_BODY()

	FProgramAttackData(): StartTime(0), Duration(0)
	{
	}

	EProgramAniAttackType AttackType = EProgramAniAttackType::Single;

	FVector StartLocation;
	FVector TargetLocation;
	FRotator StartRotation;
	FRotator TargetRotation;
	
	float StartTime;
	float Duration;
	
};

class UProjectJAttackGA;
class UWidgetComponent;
class UAbilitySystemComponent;
class UProjectJCharacterAttributeSet;

UCLASS()
class PROJECTJ_API AProjectJCharacter : public AProjectJCardBase, public IProjectJCardInterface, public IAbilitySystemInterface, public IProjectJBattleInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> FloatWidgetComponent;

	// Todo: 临时方案，以后再从道具实例中读取
	FName TempWeaponRowName = NAME_None;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// IProjectJCard interface
public:
	virtual EProjectJCardType GetCardType_Implementation() override
	{
		return EProjectJCardType::Character;
	}
	
	virtual void BindConfig_Implementation(const FName& InRowName) override;

	virtual FName GetConfigRowName_Implementation() override;
	// End IProjectJCard

	// 蓝图动画状态机
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangeAnimState(EProjectJCardAnimState InState, const FProjectJCharacterAniData& InData);

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UProjectJCharacterAttributeSet> AttributeSet;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}
	
	// ------- 战斗相关 Start -------
public:
	// 普攻技能，未装备武器时需要设置默认值； 装备武器后替换为武器默认攻击
	FGameplayAbilitySpecHandle AttackAbilitySpecHandle;
	TWeakObjectPtr<UProjectJAttackGA> AttackGA;

	bool GetIsDead_Implementation() override;
	bool GetIsAtTopTeam_Implementation() override;

	bool IsDead();
	bool IsAtTopTeam();

	void PostAfterAttackHit();
	// ------- 战斗相关 End -------

	// 程序动画
private:
	FProjectJCharacterAniData AniData;
	EProjectJCardAnimState AnimState = EProjectJCardAnimState::Idle;

	// 用于下列动画的相关数据
	FProgramAttackData GeneralAniData;
	// StartAttack动画
	FTimerHandle StartAttackTimerHandle;
	void UpdateStartAttackAnimation();

	// WaitingAttack动画
	FVector WaitingAttackOriginLocation;
	float WaitingAttackTimer = 0;

	// DoAttack动画
	void ProgramDoAttack(const FName& InName);
	// Knock动画
	FTimerHandle ProgramDoAttackTimerHandle;
	void UpdateKnockAnimation();
	bool AlreadyPostAttackHit = false;

	// Idle Return To Position Animation
	FTimerHandle IdleReturnToPositionTimerHandle;
	void UpdateIdleReturnToPositionAnimation();
};
