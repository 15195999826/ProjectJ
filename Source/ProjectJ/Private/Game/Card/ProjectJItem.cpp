// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJItem.h"

#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/ProjectJGameBFL.h"
#include "Types/Item/ProjectJPropConfig.h"

AProjectJItem::AProjectJItem()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	FloatWidgetComponent->SetupAttachment(RootComponent);
}

void AProjectJItem::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	switch (ItemType) {
		case EProjectJItemType::Weapon:
			{
				const auto ConfigRow = UProjectJGameBFL::GetWeaponConfig(ConfigRowName);
				Title->SetText(ConfigRow->Name);
			}
			break;
		case EProjectJItemType::Armor:
			{
				const auto ConfigRow = UProjectJGameBFL::GetArmorConfig(ConfigRowName);
				Title->SetText(ConfigRow->Name);
			}
			break;
		case EProjectJItemType::Prop:
			{
				const auto ConfigRow = UProjectJGameBFL::GetPropConfig(ConfigRowName);
				Title->SetText(ConfigRow->Name);
			}
			break;
		default:
			Title->SetText(FText::FromString("未知类型"));
			break;
	}
}

FName AProjectJItem::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}
