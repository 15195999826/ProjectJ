// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProjectJUIRoot.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "ProjectJ/ProjectJLogChannels.h"
#include "UI/SpecialUI/ProjectJGridSlot.h"

void UProjectJUIRoot::NativeConstruct()
{
	// auto ItemSystem = GetWorld()->GetSubsystem<UProjectJItemSystem>();
	// ItemSystem->OnItemCreated.AddUObject(this, &UProjectJUIRoot::OnItemCreated);
	// ItemSystem->OnItemDeleted.AddUObject(this, &UProjectJUIRoot::OnItemDeleted);
	// ItemSystem->OnItemUpdated.AddUObject(this, &UProjectJUIRoot::OnItemUpdated);
	Super::NativeConstruct();
}

void UProjectJUIRoot::PushUI(EProjectJUIOverlayType OverlayType, UUserWidget* Widget,
                             TEnumAsByte<EHorizontalAlignment> HorizontalAlignment, TEnumAsByte<EVerticalAlignment> VerticalAlignment) const
{
	switch (OverlayType)
	{
		case EProjectJUIOverlayType::BackEnd:
			BackEndOverlay->AddChild(Widget);
		break;
		case EProjectJUIOverlayType::Main:
			MainOverlay->AddChild(Widget);
		break;
		case EProjectJUIOverlayType::FrontEnd:
			FrontEndOverlay->AddChild(Widget);
		break;
	}
	auto OverlaySlot = CastChecked<UOverlaySlot>(Widget->Slot);
	OverlaySlot->SetHorizontalAlignment(HorizontalAlignment);
	OverlaySlot->SetVerticalAlignment(VerticalAlignment);
}

// void UProjectJUIRoot::RegisterGridSlot(int32 ID, UProjectJGridSlot* InGridSlot)
// {
// 	if (GridSlotMap.Contains(ID))
// 	{
// 		// 打印错误日志
// 		auto Parent = InGridSlot->GetParent();
// 		auto ParentName = Parent ? Parent->GetName() : TEXT("Unknown");
// 		UE_LOG(LogProjectJ, Error, TEXT("[%s].[%s], 注册ID%d失败"), *GetName(), *ParentName, ID);
// 		return;
// 	}
//
// 	GridSlotMap.Add(ID, InGridSlot);
// 	auto ItemSystem = GetWorld()->GetSubsystem<UProjectJItemSystem>();
// 	switch (InGridSlot->OwnerType) {
// 		case EProjectJGridSlotType::None:
// 			UE_LOG(LogProjectJ, Error, TEXT("ProjectJGridSlot[%s] OwnerType is None"), *InGridSlot->GetName());
// 		break;
// 		case EProjectJGridSlotType::CharacterBag:
// 			break;
// 		// case EProjectJGridSlotType::PlayerPropBag:
// 		// case EProjectJGridSlotType::PlayerWeaponBag:
// 		// case EProjectJGridSlotType::ShopTreasure:
// 		// 	{
// 		// 		auto LogicID= ItemSystem->RegisterOnePlayerSlot(InGridSlot->OwnerType);
// 		// 		UISlotToLogicSlotMap.Add(ID, LogicID);
// 		// 		LogicSlotToUISlotMap.Add(LogicID, ID);
// 		// 		UE_LOG(LogProjectJ, Log, TEXT("Register UISlot[%d] to LogicSlot[%d]"), ID, LogicID);
// 		// 	}
// 		// break;
// 	}
// }
//
// UProjectJCardWidget* UProjectJUIRoot::FindItemWidget(int32 InItemID)
// {
// }
//
// void UProjectJUIRoot::DropItemOnGridSlot(int32 InUISlotID, EProjectJGridSlotType InUISlotType,
// 	UProjectJCardWidget* ItemVC)
// {
// }
//
// UProjectJCardWidget* UProjectJUIRoot::GetItemWidget()
// {
// }
//
// void UProjectJUIRoot::RecycleItemWidget(UProjectJCardWidget* InItemWidget)
// {
// }
//
// void UProjectJUIRoot::OnItemCreated(const FProjectJItemInstance& InItemInstance)
// {
// 	auto ItemWidget = GetItemWidget();
// 	if (InItemInstance.ItemType == EProjectJItemType::Weapon)
// 	{
// 		auto DT = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous();
// 		auto WeaponRow = DT->FindRow<FProjectJWeaponConfig>(InItemInstance.ItemRowName, "UProjectJUIRoot::OnItemCreated");
// 		check(WeaponRow);
// 		ItemWidget->SetupWeaponConfig(false, InItemInstance.ItemRowName, *WeaponRow, InItemInstance.ItemID);
// 	}
// 	else
// 	{
// 		// Todo:
// 	}
// 	check(ItemWidgetMap.Contains(InItemInstance.ItemID) == false);
// 	ItemWidgetMap.Add(InItemInstance.ItemID, ItemWidget);
// 	OnItemUpdated(INDEX_NONE, InItemInstance);
// }
//
// void UProjectJUIRoot::OnItemDeleted(const FProjectJItemInstance& InItemInstance)
// {
// }
//
// void UProjectJUIRoot::OnItemUpdated(int32 OldSlotID, const FProjectJItemInstance& InItemInstance)
// {
// 	auto ItemSystem = GetWorld()->GetSubsystem<UProjectJItemSystem>();
// 	const auto SlotInfo = ItemSystem->GetLogicSlotInfo(InItemInstance.LogicSlotID);
// 	auto ItemWidget = FindItemWidget(InItemInstance.ItemID);
// 	ItemWidget->RemoveFromParent();
// 	switch (SlotInfo->SlotType) {
// 		case EProjectJGridSlotType::None:
// 			break;
// 		// case EProjectJGridSlotType::ShopTreasure:
// 		// case EProjectJGridSlotType::PlayerWeaponBag:
// 		// case EProjectJGridSlotType::PlayerPropBag:
// 		// 	{
// 		// 		auto UISlotID = LogicSlotToUISlotMap[InItemInstance.LogicSlotID];
// 		// 		UE_LOG(LogProjectJ, Log, TEXT("Item[%d] Update to UISlot[%d]"), InItemInstance.ItemID, UISlotID);
// 		// 		auto GridSlot = GridSlotMap[UISlotID];
// 		// 		GridSlot->SetContent(ItemWidget);
// 		// 	}
// 		// break;
// 		case EProjectJGridSlotType::CharacterWeapon:
// 			{
// 				
// 			}
// 		break;
// 		case EProjectJGridSlotType::CharacterBag:
// 			break;
// 		case EProjectJGridSlotType::ShopSell:
// 			break;
// 	}
// }
