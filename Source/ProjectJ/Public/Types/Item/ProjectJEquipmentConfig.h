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
enum class EProjectJAttackCapability : uint8
{
	Damage UMETA(DisplayName="伤害"),
	Heal UMETA(DisplayName="治疗")
};

UENUM()
enum class EProjectJAbilityAnimationType : uint8
{
	Program UMETA(DisplayName="程序动画"),
	Effect UMETA(DisplayName="特效"),
	Montage UMETA(DisplayName="动画蒙太奇")
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

	FProjectJAttackAbility(): AttackCapability(), TargetTeam(), AttackRange()
	{
	}

	// Todo: 通过lua脚本实现攻击能力， 这样可以方便地制作各种攻击效果
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击能力"))
	EProjectJAttackCapability AttackCapability;

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

	/* 词条 理论上层数词条不应该在这里直接赋予 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="A类词条", GetOptions="ProjectJ.ProjectJPropertyHelper.GetFeatureNames"))
	TArray<FName> Features;
	
	// Todo: 考虑到可能存在刷装备词条的可能性，这里用数组表示， 可能是表示装备可以随机到的技能脚本池子
	// Todo: 选项如果需要搜索功能，以后做
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="B类词条", GetOptions="ProjectJ.ProjectJPropertyHelper.GetLuaAbilityScriptsNames"))
	TArray<FName> AbilityLuaScriptNames;
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