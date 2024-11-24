// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/CardType.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "Types/Item/ProjectJGridSlotType.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJCardBackendSystem.generated.h"

USTRUCT(BlueprintType)
struct FProjectJCardInstance
{
	GENERATED_BODY()

	FProjectJCardInstance(): ItemID(0), CardType(), LogicSlotID(0)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	int32 ItemID;

	UPROPERTY(BlueprintReadOnly)
	FName ItemRowName;

	UPROPERTY(BlueprintReadOnly)
	EProjectJCardType CardType;
	
	UPROPERTY(BlueprintReadOnly)
	int32 LogicSlotID;
	
	// Todo: 其它可养成数据
};

USTRUCT()
struct FProjectJLogicSlotInfo
{
	GENERATED_BODY()

	FProjectJLogicSlotInfo(): SlotType(EProjectJGridSlotType::None), OwnerID(0)
	{
	}
	
	EProjectJGridSlotType SlotType;

	// 对于CharacterWeapon、CharacterBag， 该值为CharacterAICtrl的UniqueID
	int32 OwnerID;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjectJItemChangedDelegate, const FProjectJCardInstance&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProjectJItemUpdateDelegate, int32 ,const FProjectJCardInstance&);
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJCardBackendSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	int32 NextItemID = 0;
	int32 NextLogicSlotID = 0;
	
	int32 GetNextSlotID()
	{
		return NextLogicSlotID++;
	}
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	FOnProjectJItemChangedDelegate OnItemCreated;
	FOnProjectJItemChangedDelegate OnItemDeleted;
	FOnProjectJItemUpdateDelegate OnItemUpdated;
	FOnProjectJItemUpdateDelegate OnPostItemUpdated;
protected:
	TMap<int32, FProjectJCardInstance> ItemInstanceMap;
	TMap<int32, FProjectJLogicSlotInfo> LogicSlotMap;

	// 方便快速查询
	TMap<uint32, TArray<int32>> CharacterBagSlotMap;

public:
	int32 CreateItemInstance(const FName& InItemRowName, EProjectJItemType InItemType, int32 InLogicSlotID = INDEX_NONE);
	void DeleteItem(int32 InItemID);
	void DeleteItemByLogicSlotID(int32 InLogicSlotID);
	void MoveItem(int32 InItemID, int32 ToSlotID);

	// 注册槽位
	// 逻辑层
	void RegisterCharacterLogicSlots(int32 InCharacterID);
	void RemoveCharacterLogicSlots(int32 InCharacterID);
	// 对应的表演层
	int32 RegisterOnePlayerPerformSlot(EProjectJGridSlotType InSlotType);

	const FProjectJLogicSlotInfo* GetLogicSlotInfo(int32 InSlotID) const;
	const FProjectJCardInstance* FindItem(int32 InItemID) const;

private:
	int32 IntervalCreateWeaponInstance(const FName& InWeaponRowName, const FProjectJWeaponConfig* InWeaponConfig, int32 InSlotID);
	int32 IntervalCreateArmorInstance(const FName& InArmorRowName, const FProjectJArmorConfig* InArmorConfig, int32 InSlotID);

};
