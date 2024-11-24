// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameBPFL.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Game/GAS/ProjectJAttackGA.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "UI/SpecialUI/ProjectJCharacterFloatPanel.h"

void UProjectJGameBPFL::Equip(AProjectJCharacter* InCharacter, FName InRowName, EProjectJItemType InType)
{
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	auto ASC = InCharacter->GetAbilitySystemComponent();
	FProjectJAttributeGiver Giver;
	// Todo: 临时处理方案吧，最后思考怎么统一放到System处理
	// 设置角色的武器或者装备，应用属性变化；如果是武器，给与普攻技能
	switch (InType) {
		case EProjectJItemType::Weapon:
			{
				auto Config = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<FProjectJWeaponConfig>(InRowName, TEXT("Equip Weapon"));
				Giver = Config->AttributeGiver;
				// 赋予攻击攻击技能
				InCharacter->AttackAbilitySpecHandle = ASC->GiveAbility(FGameplayAbilitySpec(GSettings->AttackGAClass, 1));
				auto InstancedAbility = Cast<UProjectJAttackGA>(ASC->FindAbilitySpecFromHandle(InCharacter->AttackAbilitySpecHandle)->GetPrimaryInstance());
				InstancedAbility->SetAttackConfig(Config->AttackAbility);
				InCharacter->AttackGA = InstancedAbility;
			}
			break;
		case EProjectJItemType::Armor:
			{
				auto Config = GetDefault<UProjectJDataTableSettings>()->ArmorTable.LoadSynchronous()->FindRow<FProjectJArmorConfig>(InRowName, TEXT("Equip Armor"));
				Giver = Config->AttributeGiver;
			}
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Equip Type Error: %d"), static_cast<int32>(InType));
			Giver = FProjectJAttributeGiver();
			break;
	}

	auto AttributeSpecHandle = SimpleMakeGESpecHandle(InCharacter, GSettings->EquipmentAttributeEffect);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(AttributeSpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Health, Giver.Health);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(AttributeSpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Attack, Giver.Attack);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(AttributeSpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Speed, Giver.Speed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(AttributeSpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Armor, Giver.Armor);
	ASC->ApplyGameplayEffectSpecToSelf(*AttributeSpecHandle.Data.Get());

	auto FloatWidget =Cast<UProjectJCharacterFloatPanel>(InCharacter->FloatWidgetComponent->GetWidget());
	FloatWidget->BindEquipment(InRowName, InType);
}

void UProjectJGameBPFL::UnEquip()
{
}

FGameplayEffectSpecHandle UProjectJGameBPFL::SimpleMakeGESpecHandle(AActor* Source,
	TSubclassOf<UGameplayEffect> GEClass, float Level)
{
	const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source);
	check(ASC)
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Source);
	
	return ASC->MakeOutgoingSpec(GEClass, Level, EffectContextHandle);
}

FActiveGameplayEffectHandle UProjectJGameBPFL::SimpleApplyGEToSelf(AActor* Source, TSubclassOf<UGameplayEffect> GE,
	float Level)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source);
	if (ASC == nullptr)
	{
		return EmptyActiveGameplayEffectHandle;
	}

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Source);
	EffectContextHandle.AddInstigator(Source, Source);

	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GE, Level, EffectContextHandle);
	return ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}
