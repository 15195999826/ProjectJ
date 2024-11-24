// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI//ProjectJCharacterFloatPanel.h"

#include "Game/Card/ProjectJCharacter.h"
#include "UI/SpecialUI/ProjectJAttributeBox.h"

void UProjectJCharacterFloatPanel::BindCharacter(AProjectJCharacter* InCharacter)
{
	auto ASC = InCharacter->GetAbilitySystemComponent();
	HealthBox->Bind(ASC);
	SpeedBox->Bind(ASC);
	AttackBox->Bind(ASC);
}
