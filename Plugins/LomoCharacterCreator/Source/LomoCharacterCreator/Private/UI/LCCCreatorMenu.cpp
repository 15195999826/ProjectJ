// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LCCCreatorMenu.h"
#include "UI/LomoPCGBtn.h"
#include "LCCDeveloperSettings.h"
#include "LCCSpecieConfig.h"
#include "Components/HorizontalBox.h"

void ULCCCreatorMenu::NativeInit(UHorizontalBox* InSpecieBox)
{
	auto SpecieConfigs = GetDefault<ULCCDeveloperSettings>()->SpecieConfigs;
	
	TMap<int32, ULomoPCGBtn*> TempMap;
	TMap<int32, FLCCSpecieMenuOption*> TempSpecieConfigMap;

	for (FLCCSpecieMenuOption& SpecieConfig : SpecieConfigs)
	{
		auto SpecieButton = CreateWidget<ULomoPCGBtn>(this, SpecieConfig.SpecieButtonClass);
		auto Order = SpecieConfig.MenuOrder;
		TempMap.Add(Order, SpecieButton);
		TempSpecieConfigMap.Add(Order, &SpecieConfig);
	}
	
	// Add sorted UUserWidgets to TempSpecieBtns
	TempMap.KeySort([](int32 A, int32 B)
	{
		return A < B;
	});

	TempSpecieConfigMap.KeySort([](int32 A, int32 B)
	{
		return A < B;
	});

	InSpecieBox->ClearChildren();
	for (auto& Pair : TempMap)
	{
		auto Key = Pair.Key;
		auto SpecieButton = Pair.Value;
		FLCCSpecieMenuOption* SpecieConfig = TempSpecieConfigMap[Key];
		InSpecieBox->AddChild(Pair.Value);
		OnPCGSpecieBoxAddNewBtn(SpecieButton,
			SpecieConfig->SpecieBtnLabel, SpecieConfig->BtnIcon.LoadSynchronous(), SpecieConfig->BtnIconGlow.LoadSynchronous(),
			SpecieConfig->SpecieConfig);
	} 

	
}
