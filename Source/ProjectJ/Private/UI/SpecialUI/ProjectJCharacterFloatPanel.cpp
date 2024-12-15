// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI//ProjectJCharacterFloatPanel.h"

#include "Game/Card/ProjectJCharacter.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "UI/SpecialUI/ProjectJAttributeBox.h"

void UProjectJCharacterFloatPanel::BindCharacter(AProjectJCharacter* InCharacter)
{
	auto ASC = InCharacter->GetAbilitySystemComponent();
	HealthBox->Bind(ASC);
	SpeedBox->Bind(ASC);
	AttackBox->Bind(ASC);

	ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UProjectJCharacterFloatPanel::OnGameplayEffectApplied);
	ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UProjectJCharacterFloatPanel::OnGameplayEffectRemoved);
}

void UProjectJCharacterFloatPanel::OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	auto GrantedTags = GameplayEffectSpec.Def->GetGrantedTags();
	const auto& FirstGrantedTag = GrantedTags.Num() > 0 ? GrantedTags.GetByIndex(0) : FGameplayTag();
	if (ProjectJGameplayTags::GoodFeatureTags.Contains(FirstGrantedTag) ||
		ProjectJGameplayTags::BadFeatureTags.Contains(FirstGrantedTag) ||
		ProjectJGameplayTags::NeutralFeatureTags.Contains(FirstGrantedTag))
	{
		OnGetFeature(FirstGrantedTag);
	}
}

void UProjectJCharacterFloatPanel::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	auto GrantedTags = ActiveGameplayEffect.Spec.Def->GetGrantedTags();
	const auto& FirstGrantedTag = GrantedTags.Num() > 0 ? GrantedTags.GetByIndex(0) : FGameplayTag();
	if (ProjectJGameplayTags::GoodFeatureTags.Contains(FirstGrantedTag) ||
		ProjectJGameplayTags::BadFeatureTags.Contains(FirstGrantedTag) ||
		ProjectJGameplayTags::NeutralFeatureTags.Contains(FirstGrantedTag))
	{
		OnLoseFeature(FirstGrantedTag);
	}
}
