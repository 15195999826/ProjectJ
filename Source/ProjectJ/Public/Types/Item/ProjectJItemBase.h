﻿#pragma once
#include "CoreMinimal.h"
#include "ProjectJItemBase.generated.h"

class UPaperSprite;

UENUM(BlueprintType)
enum class EProjectJItemType : uint8
{
	None,
	Weapon UMETA(DisplayName = "武器"),
	Armor UMETA(DisplayName = "防具"),
};

USTRUCT()
struct FProjectJItemBase : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJItemBase(): ItemType()
	{
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(MultiLine=true))
	FText Description;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UPaperSprite> Sprite;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EProjectJItemType ItemType;
};