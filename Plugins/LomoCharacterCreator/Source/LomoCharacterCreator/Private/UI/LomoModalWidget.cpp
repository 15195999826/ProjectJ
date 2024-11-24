// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LomoModalWidget.h"

void ULomoModalWidget::PerformClose_Implementation(ELomoModalCloseResult Result)
{
	OnClose.Broadcast(this, Result);
}
