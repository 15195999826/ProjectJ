// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Item/ProjectJGridSlotType.h"
#include "ProjectJUIRoot.generated.h"

struct FProjectJCardInstance;
class UProjectJGridSlot;
class UProjectJCardWidget;
class UOverlay;

UENUM(BlueprintType)
enum class EProjectJUIOverlayType : uint8
{
	BackEnd,
	Main,
	FrontEnd
};

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJUIRoot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// Todo: 区分角色卡和装备卡在UI上类
	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// TSubclassOf<UProjectJCardWidget> EquipmentCardClass;
	//
	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// TSubclassOf<UProjectJCardWidget> CharacterCardClass;
	
	// UPROPERTY(BlueprintReadOnly)
	// UAutoDWMainWindow* MainWindow;
	// UPROPERTY(BlueprintReadOnly)
	// UAutoDWBattleWindow* BattleWindow;

protected:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// TSubclassOf<UAutoDWMainWindow> MainWindowClass;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// TSubclassOf<UAutoDWBattleWindow> BattleWindowClass;
	
	// Overlay, 自下而上， BackEnd , Main, FrontEnd
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> BackEndOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> MainOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> FrontEndOverlay;

	// 将不显示的Widget放在这里
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> HiddenOverlay;

	// TMap<int32, TObjectPtr<UProjectJGridSlot>> GridSlotMap;

public:
	void PushUI(EProjectJUIOverlayType OverlayType, UUserWidget* Widget, TEnumAsByte<EHorizontalAlignment> HorizontalAlignment, TEnumAsByte<EVerticalAlignment> VerticalAlignment) const;

	// void ShowImportantUI(EAutoDWImportantUI InUI);
	// void CloseImportantUI(EAutoDWImportantUI InUI);

// 	// ------ For CardWidget Start------
// public:
// 	void RegisterGridSlot(int32 ID, UProjectJGridSlot* InGridSlot);
// 	UProjectJCardWidget* FindItemWidget(int32 InItemID);
// 	void DropItemOnGridSlot(int32 InUISlotID, EProjectJGridSlotType InUISlotType, UProjectJCardWidget* ItemVC);
// 	
// protected:
// 	TMap<int32, int32> LogicSlotToUISlotMap;
// 	TMap<int32, int32> UISlotToLogicSlotMap;
//
// 	UPROPERTY()
// 	TQueue<TObjectPtr<UProjectJCardWidget>> CardWidgetPool;
//
// 	UPROPERTY()
// 	TMap<int32, TObjectPtr<UProjectJCardWidget>> CardWidgetMap;
// 	
// private:
// 	UProjectJCardWidget* GetItemWidget();
// 	
// 	void RecycleItemWidget(UProjectJCardWidget* InItemWidget);
//
// 	void OnItemCreated(const FProjectJItemInstance& InItemInstance);
// 	void OnItemDeleted(const FProjectJItemInstance& InItemInstance);
// 	void OnItemUpdated(int32 OldSlotID, const FProjectJItemInstance& InItemInstance);
//
// 	// ------ For CardWidget End------
};
