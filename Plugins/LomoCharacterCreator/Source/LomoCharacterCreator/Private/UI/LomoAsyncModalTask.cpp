// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LomoAsyncModalTask.h"

#include "Kismet/GameplayStatics.h"
#include "UI/LomoUITypes.h"

ULomoAsyncModalTask* ULomoAsyncModalTask::AsyncShowModalFromClass(const UObject* WorldContextObject,
                                                                  TSubclassOf<ULomoModalWidget> Class, FText InMessage)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	ULomoModalWidget* Widget = CreateWidget<ULomoModalWidget>(
							  GameInstance, Class);
	Widget->Setup(InMessage);
	// Call default function
	return AsyncShowModalFromRef(WorldContextObject, Widget);
}

ULomoAsyncModalTask* ULomoAsyncModalTask::AsyncShowModalFromRef(const UObject* WorldContextObject,
	ULomoModalWidget* ModalBaseWidget)
{
	ULomoAsyncModalTask* BlueprintNode = NewObject<ULomoAsyncModalTask>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->ModalRef = ModalBaseWidget;
	// Add to viewport
	ModalBaseWidget->AddToViewport();
	// Return
	return BlueprintNode;
}

void ULomoAsyncModalTask::NativeOnClose(ULomoModalWidget* UserWidget, ELomoModalCloseResult Result)
{
	OnClose.Broadcast(UserWidget, Result);
}

void ULomoAsyncModalTask::Activate()
{
	Super::Activate();

	// Bind
	ModalRef->OnClose.AddDynamic(this, &ULomoAsyncModalTask::NativeOnClose);

	// Call on show event for custom stuff
	OnShow.Broadcast(ModalRef, ELomoModalCloseResult::Unknown);
}

UWorld* ULomoAsyncModalTask::GetWorld() const
{
	return WorldContextObject->GetWorld();
}
