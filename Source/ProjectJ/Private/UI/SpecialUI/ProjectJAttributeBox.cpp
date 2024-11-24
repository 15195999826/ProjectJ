// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/ProjectJAttributeBox.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Components/TextBlock.h"

void UProjectJAttributeBox::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ProjectJGameplayTags::Attributes2Text.Contains(AttributeTag))
	{
		AttributeName->SetText(ProjectJGameplayTags::Attributes2Text[AttributeTag]);
	}
	else
	{
		AttributeName->SetText(FText::FromString(AttributeTag.ToString()));
	}
	
	if (ProjectJGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
	{
		AttributeSpace->SetVisibility(ESlateVisibility::Visible);
		MaxAttributeValue->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AttributeSpace->SetVisibility(ESlateVisibility::Collapsed);
		MaxAttributeValue->SetVisibility(ESlateVisibility::Collapsed);
	}

	if(ShowText)
	{
		AttributeName->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		AttributeName->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UseAttrMinusMaxAsValue)
	{
		MaxAttributeValue->SetVisibility(ESlateVisibility::Collapsed);
		AttributeSpace->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UProjectJAttributeBox::Bind(UAbilitySystemComponent* InASC)
{
	if (!ProjectJGameplayTags::Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}

	const FGameplayAttribute& Attribute = ProjectJGameplayTags::Tag2Attribute[AttributeTag]();
	
	RemoveListeners(Attribute);
	
	WeakASCCache = InASC;

	// 直接进行一次更新
	bool Found = false;
	auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(Attribute, Found);

	FText CustomText;
	bool bCustom = false;
	CustomValueText(AttributeTag, CurrentValue, bCustom, CustomText);
	if (bCustom)
	{
		AttributeValue->SetText(CustomText);
	}
	else
	{
		AttributeValue->SetText(FText::AsNumber(CurrentValue));
	}


	WeakASCCache->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
		this, &UProjectJAttributeBox::OnAttributeChanged);
	if (ProjectJGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
	{
		bHasMaxAttribute = true;
		const auto& MaxAttribute = ProjectJGameplayTags::Tag2Attribute[MaxAttributeTag]();
		Found = false;
		auto CurrentMaxValue = WeakASCCache->GetGameplayAttributeValue(MaxAttribute, Found);

		if (!UseAttrMinusMaxAsValue)
		{
			CustomValueText(MaxAttributeTag, CurrentMaxValue, bCustom, CustomText);
			if (bCustom)
			{
				MaxAttributeValue->SetText(CustomText);
			}
			else
			{
				MaxAttributeValue->SetText(FText::AsNumber(CurrentMaxValue));
			}
		}
		else
		{
			AttributeValue->SetText(FText::AsNumber(CurrentValue - CurrentMaxValue));
		}
		
		WeakASCCache->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(
			this, &UProjectJAttributeBox::OnMaxAttributeChanged);
	}
}

void UProjectJAttributeBox::UnBind()
{
	if (!ProjectJGameplayTags::Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}

	RemoveListeners(ProjectJGameplayTags::Tag2Attribute[AttributeTag]());
	WeakASCCache = nullptr;
}

void UProjectJAttributeBox::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!UseAttrMinusMaxAsValue)
	{
		FText CustomText;
		bool bCustom = false;
		CustomValueText(AttributeTag, OnAttributeChangeData.NewValue, bCustom, CustomText);
		if (bCustom)
		{
			AttributeValue->SetText(CustomText);
			return;
		}
	
		AttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue));
	}
	else
	{
		if (WeakASCCache.IsValid())
		{
			bool Found = false;
			auto MaxValue = WeakASCCache->GetGameplayAttributeValue(ProjectJGameplayTags::Tag2Attribute[MaxAttributeTag](), Found);
			AttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue - MaxValue));
		}
	}
}

void UProjectJAttributeBox::OnMaxAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	check(bHasMaxAttribute);
	if (!UseAttrMinusMaxAsValue)
	{
		FText CustomText;
		bool bCustom = false;
		CustomValueText(MaxAttributeTag, OnAttributeChangeData.NewValue, bCustom, CustomText);
		if (bCustom)
		{
			MaxAttributeValue->SetText(CustomText);
			return;
		}
		MaxAttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue));
	}
	else
	{
		if (WeakASCCache.IsValid())
		{
			bool Found = false;
			auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(ProjectJGameplayTags::Tag2Attribute[AttributeTag](), Found);
			auto MaxValue = OnAttributeChangeData.NewValue;
			AttributeValue->SetText(FText::AsNumber(CurrentValue - MaxValue));
		}
	}
	
	
}

void UProjectJAttributeBox::RemoveListeners(const FGameplayAttribute& Attribute)
{
	if (WeakASCCache.IsValid())
	{
		WeakASCCache->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		if (ProjectJGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
		{
			const auto& MaxAttribute = ProjectJGameplayTags::Tag2Attribute[MaxAttributeTag]();
			WeakASCCache->GetGameplayAttributeValueChangeDelegate(MaxAttribute).RemoveAll(this);
		}
	}
}
