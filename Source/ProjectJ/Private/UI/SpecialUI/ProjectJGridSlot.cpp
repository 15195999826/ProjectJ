// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/ProjectJGridSlot.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/NamedSlot.h"
#include "Core/ProjectJGameInstance.h"
#include "UI/ProjectJUIRoot.h"
#include "UI/SpecialUI/ProjectJCardWidget.h"

//
// void UProjectJGridSlot::NativePreConstruct()
// {
// 	if (!IsDesignTime())
// 	{
// 		auto ProjectJGameInstance = GetWorld()->GetGameInstance<UProjectJGameInstance>();
// 		ProjectJGameInstance->UIRoot->RegisterGridSlot(ID, this);
// 	}
// 	Super::NativePreConstruct();
// }
//
// bool UProjectJGridSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
//                                  UDragDropOperation* InOperation)
// {
// 	auto ItemVC = Cast<UProjectJCardWidget>(InOperation->Payload);
// 	if(CanDropOnSlot(ItemVC))
// 	{
// 		auto ProjectJGameInstance = GetWorld()->GetGameInstance<UProjectJGameInstance>();
// 		ProjectJGameInstance->UIRoot->DropItemOnGridSlot(ID, OwnerType, ItemVC);
// 	}
// 	
// 	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
// }
//
// void UProjectJGridSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
// 	UDragDropOperation* InOperation)
// {
// 	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
// }
//
// void UProjectJGridSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
// {
// 	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
// }
//
// void UProjectJGridSlot::SetContent(UProjectJCardWidget* ItemWidget)
// {
// 	DropOnSlot->RemoveChildAt(0);
// 	
// 	ItemWidget->SetVisibility(ESlateVisibility::Visible);
// 	DropOnSlot->AddChild(ItemWidget);
// }



