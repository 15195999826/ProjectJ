// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Types/ProjectJCharacterConfig.h"
#include "UI/SpecialUI/ProjectJCharacterFloatPanel.h"

// Sets default values
AProjectJCharacter::AProjectJCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	// 手动设置ReceiveHardwareInput为true
	FloatWidgetComponent->SetupAttachment(Mesh, "Cube");


	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	AttributeSet = CreateDefaultSubobject<UProjectJCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AProjectJCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 初始ASC
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

// Called every frame
void AProjectJCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectJCharacter::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	Title->SetText(FText::FromString(InRowName.ToString()));
	auto Config = GetDefault<UProjectJDataTableSettings>()->CharacterTable.LoadSynchronous()->FindRow<
		FProjectJCharacterConfig>(InRowName, TEXT(""));

	if (Config)
	{
		// 给予属性
		auto GSettings = GetDefault<UProjectJGeneralSettings>();
		auto InitAttributeGESpecHandle = UProjectJGameBPFL::SimpleMakeGESpecHandle(this, GSettings->InitAttributeEffect);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Health, Config->Health);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Attack, Config->Attack);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Speed, Config->Speed);

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitAttributeGESpecHandle.Data.Get());

		auto FloatPanel = Cast<UProjectJCharacterFloatPanel>(FloatWidgetComponent->GetWidget());
		FloatPanel->BindCharacter(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterConfig %s not found"), *InRowName.ToString());
	}
}

FName AProjectJCharacter::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}

bool AProjectJCharacter::IsDead()
{
	auto Health = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetHealthAttribute());
	auto Damage = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetDamageAttribute());
	return Health <= Damage;
}

