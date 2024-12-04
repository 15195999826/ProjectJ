#pragma once
#include "CoreMinimal.h"
#include "ProjectJItemBase.h"
#include "Types/ProjectJAttackRange.h"
#include "Types/ProjectJBattleStage.h"
#include "ProjectJEquipmentConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJAttributeGiver
{
	GENERATED_BODY()

	FProjectJAttributeGiver(): Attack(0), Health(0), Speed(0), Armor(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击力"))
	int32 Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="血量"))
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="身法"))
	int32 Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="护甲"))
	int32 Armor;
};

UENUM()
enum class EProjectJAbilityAnimationType : uint8
{
	Program UMETA(DisplayName="程序动画"),
	Effect UMETA(DisplayName="特效")
};

USTRUCT(BlueprintType)
struct FProjectJAttackAbilityAnimationConfig
{
	GENERATED_BODY()
	
	FProjectJAttackAbilityAnimationConfig()
	{
		
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="动画类型"))
	EProjectJAbilityAnimationType AnimationType = EProjectJAbilityAnimationType::Program;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="表演阶段"))
	EProjectJBattleStage PerformStage = EProjectJBattleStage::CharacterDoAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="是否携带攻击点"))
	bool GiveAttackPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="资源", EditConditionHides, EditCondition="AnimationType != EProjectJAbilityAnimationType::Program"))
	TSubclassOf<UObject> Resource;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="自定义字段(暂时没用)",GetKeyOptions="ProjectJ.ProjectJPropertyHelper.GetAnimCustomKeys"))
	TMap<FName, FString> CustomKV; 
};

USTRUCT(BlueprintType)
struct FProjectJAttackAbility
{
	GENERATED_BODY()

	FProjectJAttackAbility(): TargetTeam(), AttackRange()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="目标队伍"))
	EProjectJTargetTeam TargetTeam;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击范围"))
	EProjectJAttackRange AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="动画配置"))
	TArray<FProjectJAttackAbilityAnimationConfig> AnimationConfigs;
};


USTRUCT()
struct FProjectJEquipmentConfig : public FProjectJItemBase
{
	GENERATED_BODY()

	FProjectJEquipmentConfig()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="属性"))
	FProjectJAttributeGiver AttributeGiver;
	// 耐久度。 Todo: 考虑实现方案
};

USTRUCT()
struct FProjectJWeaponConfig : public FProjectJEquipmentConfig
{
	GENERATED_BODY()

	FProjectJWeaponConfig()
	{
		ItemType = EProjectJItemType::Weapon;
	}

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击配置"))
	FProjectJAttackAbility AttackAbility;
	// 耐久度。 Todo: 考虑实现方案
};

USTRUCT()
struct FProjectJArmorConfig : public FProjectJEquipmentConfig
{
	GENERATED_BODY()

	FProjectJArmorConfig()
	{
		ItemType = EProjectJItemType::Armor;
	}

	// 耐久度。 Todo: 考虑实现方案
	
};