// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardBackendSystem.h"

#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"

void UProjectJCardBackendSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

int32 UProjectJCardBackendSystem::CreateItemInstance(const FName& InItemRowName, EProjectJItemType InItemType,
                                              int32 InLogicSlotID)
{
	switch (InItemType) {
		case EProjectJItemType::None:
			break;
		case EProjectJItemType::Weapon:
			{
				auto DT = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous();
				auto Config = DT->FindRow<FProjectJWeaponConfig>(InItemRowName, "CreateItemInstance");
				check(Config);
				return IntervalCreateWeaponInstance(InItemRowName, Config, InLogicSlotID);
			}
			
		case EProjectJItemType::Armor:
			{
				auto DT = GetDefault<UProjectJDataTableSettings>()->ArmorTable.LoadSynchronous();
				auto Config = DT->FindRow<FProjectJArmorConfig>(InItemRowName, "CreateItemInstance");
				check(Config);
				return IntervalCreateArmorInstance(InItemRowName, Config, InLogicSlotID);
			}
	}

	check(false);
	return INDEX_NONE;
}

void UProjectJCardBackendSystem::DeleteItem(int32 InItemID)
{
}

void UProjectJCardBackendSystem::DeleteItemByLogicSlotID(int32 InLogicSlotID)
{
	for (auto& Pair : ItemInstanceMap)
	{
		if (Pair.Value.LogicSlotID == InLogicSlotID)
		{
			DeleteItem(Pair.Key);
			break;
		}
	}
}

void UProjectJCardBackendSystem::MoveItem(int32 InItemID, int32 ToSlotID)
{
}

void UProjectJCardBackendSystem::RegisterCharacterLogicSlots(int32 InCharacterID)
{
	TArray<int32> BagSlotIDs;
	for (int32 i = 0; i < 2; i++)
	{
		// Todo: 根据目前的设定, 0是武器，1是防具
		FProjectJLogicSlotInfo SlotInfo;
		SlotInfo.SlotType = EProjectJGridSlotType::CharacterBag;
		SlotInfo.OwnerID = InCharacterID;
		auto ID = GetNextSlotID();
		LogicSlotMap.Add(ID, SlotInfo);
		BagSlotIDs.Add(ID);
	}
	CharacterBagSlotMap.Add(InCharacterID, BagSlotIDs);
}

void UProjectJCardBackendSystem::RemoveCharacterLogicSlots(int32 InCharacterID)
{
	for (auto& Pair : LogicSlotMap)
	{
		if (Pair.Value.SlotType == EProjectJGridSlotType::CharacterBag && Pair.Value.OwnerID == InCharacterID)
		{
			LogicSlotMap.Remove(Pair.Key);
		}
	}
}

int32 UProjectJCardBackendSystem::RegisterOnePlayerPerformSlot(EProjectJGridSlotType InSlotType)
{
	// Todo: 这里原先的设计是，UI上存在格子， UI上构造格子时， 就会主动调用这个函数，同时生成一个逻辑层面的槽位.
	auto ID = GetNextSlotID();
	FProjectJLogicSlotInfo SlotInfo;
	SlotInfo.SlotType = EProjectJGridSlotType::PlayerBag;
	LogicSlotMap.Add(ID, SlotInfo);
	return ID;
}

const FProjectJLogicSlotInfo* UProjectJCardBackendSystem::GetLogicSlotInfo(int32 InSlotID) const
{
	return LogicSlotMap.Find(InSlotID);
}

const FProjectJCardInstance* UProjectJCardBackendSystem::FindItem(int32 InItemID) const
{
	return ItemInstanceMap.Find(InItemID);
}

int32 UProjectJCardBackendSystem::IntervalCreateWeaponInstance(const FName& InWeaponRowName,
	const FProjectJWeaponConfig* InWeaponConfig, int32 InSlotID)
{
	NextItemID++;
	FProjectJCardInstance WeaponInstance;
	WeaponInstance.ItemID = NextItemID;
	WeaponInstance.ItemRowName = InWeaponRowName;
	// WeaponInstance.CardType = EProjectJItemType::Weapon;
	WeaponInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, WeaponInstance);
	OnItemCreated.Broadcast(WeaponInstance);
	return NextItemID;
}

int32 UProjectJCardBackendSystem::IntervalCreateArmorInstance(const FName& InArmorRowName,
	const FProjectJArmorConfig* InArmorConfig, int32 InSlotID)
{
	NextItemID++;
	FProjectJCardInstance PropInstance;
	PropInstance.ItemID = NextItemID;
	PropInstance.ItemRowName = InArmorRowName;
	// PropInstance.CardType = EProjectJItemType::Armor;
	PropInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, PropInstance);
	OnItemCreated.Broadcast(PropInstance);
	return NextItemID;
}
