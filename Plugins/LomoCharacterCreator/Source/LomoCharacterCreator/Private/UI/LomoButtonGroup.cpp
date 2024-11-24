// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LomoButtonGroup.h"

#include "Components/PanelWidget.h"
#include "LomoCharacterCreator/LCCLogChannels.h"

void ULomoButtonGroup::BtnGroupNativePreConstruct(UPanelWidget* InButtonContainer)
{
	ButtonContainer = InButtonContainer;
	
	for (int32 i = 0; i < ButtonContainer->GetChildrenCount(); ++i)
	{
		ULomoGroupButton* Btn = Cast<ULomoGroupButton>(ButtonContainer->GetChildAt(i));
		if (Btn)
		{
			Btn->OnGroupButtonActive.AddUObject(this, &ULomoButtonGroup::OnButtonActive);
		}
	}
}

void ULomoButtonGroup::ClearGroup()
{
	// 移除所有监听
	for (int32 i = 0; i < ButtonContainer->GetChildrenCount(); ++i)
	{
		ULomoGroupButton* Btn = Cast<ULomoGroupButton>(ButtonContainer->GetChildAt(i));
		if (Btn)
		{
			Btn->OnGroupButtonActive.RemoveAll(this);
		}
	}
	ButtonContainer->ClearChildren();
}

void ULomoButtonGroup::PcgButtonGroup(TSubclassOf<ULomoGroupButton> BtnClass, TArray<FLomoGroupBtnConfig> InMenuConfig)
{
	ClearGroup();
	
	for (const FLomoGroupBtnConfig& BtnConfig : InMenuConfig)
	{
		AppendPcgButton(BtnClass, BtnConfig);
	}
}

ULomoGroupButton* ULomoButtonGroup::AppendPcgButton(TSubclassOf<ULomoGroupButton> BtnClass, FLomoGroupBtnConfig InBtnConfig)
{
	ULomoGroupButton* Btn = CreateWidget<ULomoGroupButton>(this, BtnClass);
	Btn->SetConfig(InBtnConfig);
	ButtonContainer->AddChild(Btn);
	CustomPcgBtnStyle(Btn);
	Btn->OnGroupButtonActive.AddUObject(this, &ULomoButtonGroup::OnButtonActive);

	return Btn;
}

void ULomoButtonGroup::DeActiveAll()
{
	for (int32 i = 0; i < ButtonContainer->GetChildrenCount(); ++i)
	{
		ULomoGroupButton* Btn = Cast<ULomoGroupButton>(ButtonContainer->GetChildAt(i));
		if (Btn)
		{
			Btn->DeActive();
		}
	}
}

void ULomoButtonGroup::SetDefaultActiveButton(int32 InIndex)
{
	ULomoGroupButton* Btn = Cast<ULomoGroupButton>(ButtonContainer->GetChildAt(InIndex));
	if (Btn)
	{
		Btn->Active();
	}
}

void ULomoButtonGroup::OnButtonActive(ULomoGroupButton* InGroupButton, int32 InIntPayload)
{
	if (ActiveButton.IsValid())
	{
		ActiveButton->DeActive();
	}

	ActiveButton = InGroupButton;

	OnChangeActiveButton.Broadcast(InIntPayload);
}