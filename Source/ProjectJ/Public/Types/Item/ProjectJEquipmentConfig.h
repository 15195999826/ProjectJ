#pragma once
#include "CoreMinimal.h"
#include "ProjectJItemBase.h"
#include "Types/ProjectJAttackRange.h"
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

USTRUCT(BlueprintType)
struct FProjectJAttackAbility
{
	GENERATED_BODY()

	FProjectJAttackAbility(): AttackRange()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击范围"))
	EProjectJAttackRange AttackRange;

	// Todo: 自定义数据配置模板， 给与动画配置， Key Value只能是给定的值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="普攻", meta=(DisplayName="攻击动画配置"))
	TMap<FName, FString> AnimationConfig; 
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