// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Item/ProjectJGridSlotType.h"
#include "ProjectJGridSlot.generated.h"

class UProjectJCardWidget;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGridSlot : public UUserWidget
{
	GENERATED_BODY()
// protected:
// 	virtual void NativePreConstruct() override;
// 	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
// 	virtual void NativeOnDragEnter( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
// 	virtual void NativeOnDragLeave( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
//
// public:
// 	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
// 	TObjectPtr<UNamedSlot> DropOnSlot;
//
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
// 	int32 ID;
//
// 	// 表子示自己是什么位置的格
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
// 	EProjectJGridSlotType OwnerType;
//
// 	void SetContent(UProjectJCardWidget* ItemWidget);
// 	// UFUNCTION(BlueprintImplementableEvent)
// 	// void NativeCallOnChanged();
// 	//
// 	// UFUNCTION(BlueprintImplementableEvent)
// 	// void NativeCallInit();
// 	//
// 	
// 	//
// 	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
// 	// void DoHighlight();
// 	//
// 	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
// 	// void DeHighlight();
//
// protected:
// 	UFUNCTION(BlueprintImplementableEvent)
// 	bool CanDropOnSlot(UProjectJCardWidget* InItemWidget);
// 	
// 	// UFUNCTION(BlueprintCallable, BlueprintPure)
// 	// bool IsInnerItem(int ItemId);
// 	
};
