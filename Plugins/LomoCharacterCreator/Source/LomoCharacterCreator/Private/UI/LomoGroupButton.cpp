// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LomoGroupButton.h"

void ULomoGroupButton::Active_Implementation()
{
	if (bActive)
	{
		return;
	}
	
	bActive = true;
	OnGroupButtonActive.Broadcast(this, Config.IntPayload);
}

void ULomoGroupButton::DeActive_Implementation()
{
	bActive = false;
}

void ULomoGroupButton::SetConfig(FLomoGroupBtnConfig InConfig)
{
	Config = InConfig;
}
