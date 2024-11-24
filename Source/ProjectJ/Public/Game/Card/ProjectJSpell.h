// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "ProjectJSpell.generated.h"

class UTextRenderComponent;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJSpell : public AProjectJCardBase, public IProjectJCardInterface
{
	GENERATED_BODY()

public:

	// IProjectJCard interface
public:
	virtual EProjectJCardType GetCardType_Implementation() override
	{
		return EProjectJCardType::Spell;
	}
	
	virtual void BindConfig_Implementation(const FName& InRowName) override;

	virtual FName GetConfigRowName_Implementation() override;

	// Drag Interface
public:
	virtual void OnDragStart() override;
	virtual void OnDrop(float InDuration) override;
	virtual void OnCancelDrag() override;

private:
	FTransform DragStartRelativeTransform;
};