// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameBFL.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJLuaExecutor.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Game/GAS/ProjectJAttackGA.h"
#include "Game/GAS/ProjectJGameplayEffectContext.h"
#include "Game/GAS/ProjectJLuaGameplayAbility.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "Game/GAS/ProjectJGameplayEffectUIData.h"
#include "UI/SpecialUI/ProjectJCharacterFloatPanel.h"

void UProjectJGameBFL::Equip(AProjectJCharacter* InCharacter, FName InRowName, EProjectJItemType InType)
{
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	auto ASC = InCharacter->GetAbilitySystemComponent();
	TArray<FName> GivenFeature;
	TArray<FName> GivenLuaAbilityScripts;
	FProjectJAttributeGiver Giver;
	// Todo: 临时处理方案吧，最后思考怎么统一放到System处理
	// 设置角色的武器或者装备，应用属性变化；如果是武器，给与普攻技能
	switch (InType) {
		case EProjectJItemType::Weapon:
			{
				InCharacter->TempWeaponRowName = InRowName;
				auto Config = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<FProjectJWeaponConfig>(InRowName, TEXT("Equip Weapon"));
				Giver = Config->AttributeGiver;
				GivenFeature = Config->Features;
				GivenLuaAbilityScripts = Config->AbilityLuaScriptNames;
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
				GivenFeature = Config->Features;
				GivenLuaAbilityScripts = Config->AbilityLuaScriptNames;
			}
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Equip Type Error: %d"), static_cast<int32>(InType));
			Giver = FProjectJAttributeGiver();
			break;
	}

	// 给与持续无限回合的特性
	for (const auto& Feature : GivenFeature)
	{
		auto Tag = UGameplayTagsManager::Get().RequestGameplayTag(Feature);
		InCharacter->GetFeature(Tag, 1, -1);
	}
	
	// 给予B类词条对应的技能
	for (const auto& LuaScript : GivenLuaAbilityScripts)
	{
		InCharacter->LuaAbility->RegisterAbility(LuaScript);
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

void UProjectJGameBFL::UnEquip()
{
}

FGameplayEffectSpecHandle UProjectJGameBFL::SimpleMakeGESpecHandle(AActor* Source,
	TSubclassOf<UGameplayEffect> GEClass, float Level)
{
	const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source);
	check(ASC)
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Source);
	
	return ASC->MakeOutgoingSpec(GEClass, Level, EffectContextHandle);
}

FText UProjectJGameBFL::GetLuaAbilityDesc(const UObject* WorldContextObject, const FName& InLuaScriptCHSName)
{
	auto LuaExecutor = WorldContextObject->GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
	if (LuaExecutor)
	{
		const auto& LuaScriptsMap = GetDefault<UProjectJPropertyHelper>()->CHS2AbilityLuaScriptNameMap;
		if (LuaScriptsMap.Contains(InLuaScriptCHSName))
		{
			return LuaExecutor->GetLuaAbilityDesc(LuaScriptsMap[InLuaScriptCHSName]);
		}
	}

	// 返回错误配置
	return FText::FromString(TEXT("Error LuaScriptCHSName"));
}

FActiveGameplayEffectHandle UProjectJGameBFL::SimpleApplyGEToSelf(AActor* Source, TSubclassOf<UGameplayEffect> GE,
                                                                  float Level, int32 Round)
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
	auto GEContext = static_cast<FProjectJGameplayEffectContext*>(EffectSpecHandle.Data->GetContext().Get());
	GEContext->SetDuration(Round);
	return ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

FProjectJBattleEventData UProjectJGameBFL::TryGetProjectJGameEventData(const FGameplayEventData& InData,
	EProjectJValid& OutValid)
{
	FGameplayAbilityTargetData* data = InData.TargetData.Data[0].Get();
	// Valid check we have something to use :D
	if(data == nullptr || data->GetScriptStruct() != FProjectJBattleEventData::StaticStruct())
	{
		OutValid = EProjectJValid::Invalid;
		return FProjectJBattleEventData();
	}
	
	FProjectJBattleEventData* CustomData = static_cast<FProjectJBattleEventData*>(data);
	OutValid = EProjectJValid::Valid;
	return *CustomData;
}

const UProjectJGameplayEffectUIData* UProjectJGameBFL::GetGEUIData(const FGameplayTag& InTag, EProjectJValid& OutValid)
{
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	if (GSettings->FeatureGEMap.Contains(InTag))
	{
		auto GEClass = GSettings->FeatureGEMap[InTag];
		auto GE = GEClass.GetDefaultObject();
		auto Com = GE->FindComponent<UProjectJGameplayEffectUIData>();
		if (Com)
		{
			OutValid = EProjectJValid::Valid;
			return Com;
		}
	}
	OutValid = EProjectJValid::Invalid;
	return nullptr;
}

FGameplayTag UProjectJGameBFL::RequestGameplayTag(const FName& InTagName)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(InTagName, false);
}

FProjectJWeaponConfig* UProjectJGameBFL::GetWeaponConfig(const FName& InRowName)
{
	auto DT = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous();
	if (auto Config = DT->FindRow<FProjectJWeaponConfig>(InRowName, TEXT("Get Weapon Config")))
	{
		return Config;
	}
	
	return nullptr;
}

FProjectJArmorConfig* UProjectJGameBFL::GetArmorConfig(const FName& InRowName)
{
	auto DT = GetDefault<UProjectJDataTableSettings>()->ArmorTable.LoadSynchronous();
	if (auto Config = DT->FindRow<FProjectJArmorConfig>(InRowName, TEXT("Get Armor Config")))
	{
		return Config;
	}
	
	return nullptr;
}

FProjectJPropConfig* UProjectJGameBFL::GetPropConfig(const FName& InRowName)
{
	auto DT = GetDefault<UProjectJDataTableSettings>()->PropTable.LoadSynchronous();
	if (auto Config = DT->FindRow<FProjectJPropConfig>(InRowName, TEXT("Get Prop Config")))
	{
		return Config;
	}
	
	return nullptr;
}

FProjectJUtilityConfig* UProjectJGameBFL::GetUtilityConfig(const FName& InRowName)
{
	auto DT = GetDefault<UProjectJDataTableSettings>()->UtilityTable.LoadSynchronous();
	if (auto Config = DT->FindRow<FProjectJUtilityConfig>(InRowName, TEXT("Get Utility Config")))
	{
		return Config;
	}
	
	return nullptr;
}
