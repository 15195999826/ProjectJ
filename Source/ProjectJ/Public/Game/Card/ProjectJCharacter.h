// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "ProjectJCardBase.h"
#include "Interface/ProjectJBattleInterface.h"
#include "Interface/ProjectJCardInterface.h"
#include "Types/ProjectJCardAnimState.h"
#include "ProjectJCharacter.generated.h"

class UProjectJAbilitySystemComponent;
class UProjectJLuaGameplayAbility;

UENUM()
enum class EProgramAniAttackType
{
	Single,
	
};

USTRUCT()
struct FProgramAnimationData
{
	GENERATED_BODY()

	FProgramAnimationData(): StartTime(0), Duration(0)
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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInstanceDynamic> BackgroundDynamicMaterial;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}
	
	// ------- 战斗相关 BattleInterface Start -------
public:
	// 普攻技能，未装备武器时需要设置默认值； 装备武器后替换为武器默认攻击
	UPROPERTY()
	TObjectPtr<UProjectJLuaGameplayAbility> LuaAbility;

	FGameplayAbilitySpecHandle AttackAbilitySpecHandle;
	TWeakObjectPtr<UProjectJAttackGA> AttackGA;
	
	TWeakObjectPtr<UProjectJAbilitySystemComponent> ProjectJASC;

	bool GetIsDead_Implementation() override;
	bool GetIsAtTopTeam_Implementation() override;

	bool IsDead();
	bool IsAtTopTeam();

	void PostAfterAttackHit();

	// 特性(A类词条）
	/**
	 * 特性的功能实现
	 * 1. 特性GE应当唯一存在
	 * 2. 给与特性标签
	 * 3. 检查是否存在对应的Lua脚本, 存在则RegisterAbility， 并将EventID额外记录到CachedFeatureEventIDs
	 * 4. 区分LooseFeatureTags和FeatureGE的原因是:
	 *	  -有些纯逻辑的功能， 非外显的词条Tag， 无需对应的GE， 简化功能制作流程
	 *	  -特性Tag则总有一个对应的GE， 是游戏内定义好的词条
	 * @param InFeatureTag
	 * @param InCount
	 * @param InRound 持续回合数， -1表示永久
	 */
	void GetFeature(const FGameplayTag& InFeatureTag, int32 InCount, int32 InRound);

protected:
	// 缓存的特性EventID， 只有该特性使用了Lua脚本实现， 这里才会存在对应的EventID 
	TMap<FGameplayTag, int32> CachedFeatureEventIDs;
	
	// ------- 战斗相关 End -------

	// 程序动画
private:
	FProjectJCharacterAniData AniData;
	EProjectJCardAnimState AnimState = EProjectJCardAnimState::Idle;

	// 用于下列动画的相关数据
	FProgramAnimationData GeneralAniData;
	// StartAttack动画
	FTimerHandle StartAttackTimerHandle;
	void UpdateStartAttackAnimation();

	// WaitingAttack动画
	FVector WaitingAttackOriginLocation;
	float WaitingAttackTimer = 0;

	// DoAttack动画, 返回后摇时长
	float ProgramDoAttack(const FName& InName);
	// Knock动画
	FTimerHandle ProgramDoAttackTimerHandle;
	void UpdateKnockAnimation();
	bool AlreadyPostAttackHit = false;

	// Idle Return To Position Animation
	FTimerHandle IdleReturnToPositionTimerHandle;
	void UpdateIdleReturnToPositionAnimation();

	// 卡背溶解动画
	FTimerHandle DissolveTimerHandle;
	void UpdateDissolveAnimation();
};
