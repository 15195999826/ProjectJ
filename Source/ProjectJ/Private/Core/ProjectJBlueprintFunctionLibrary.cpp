// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ProjectJBlueprintFunctionLibrary.h"

#include "Core/ProjectJGameInstance.h"
#include "Framework/Application/NavigationConfig.h"
#include "Kismet/GameplayStatics.h"

void UProjectJBlueprintFunctionLibrary::EnableUINav()
{
	FNavigationConfig& NavConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavConfig.bTabNavigation = true;
}

void UProjectJBlueprintFunctionLibrary::DisableUINav()
{
	FNavigationConfig& NavConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavConfig.bTabNavigation = false;
}

FText UProjectJBlueprintFunctionLibrary::GetLocText(const FString& InNameSpace, const FString& InKey, const FText& InDefault)
{
	FText Out;
	if (FText::FindText(InNameSpace, *InKey, Out))
	{
		return Out;
	}
	
	return InDefault;
}

void UProjectJBlueprintFunctionLibrary::PushUI(const UObject* WorldContextObject, EProjectJUIOverlayType OverlayType,
	UUserWidget* Widget, TEnumAsByte<EHorizontalAlignment> HorizontalAlignment,
	TEnumAsByte<EVerticalAlignment> VerticalAlignment)
{
	auto AutoDWGI = Cast<UProjectJGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	AutoDWGI->UIRoot->PushUI(OverlayType, Widget, HorizontalAlignment, VerticalAlignment);
}
