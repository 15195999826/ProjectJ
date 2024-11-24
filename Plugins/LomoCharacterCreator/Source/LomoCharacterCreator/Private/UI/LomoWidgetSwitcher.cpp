// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LomoWidgetSwitcher.h"

void ULomoWidgetSwitcher::SetSlotVisibility(int32 Index)
{
	if ( MyWidgetSwitcher.IsValid() )
	{
		// Ensure the index is clamped to a valid range.
		const int32 SafeIndex = FMath::Clamp(Index, 0, FMath::Max(0, Slots.Num() - 1));
		MyWidgetSwitcher->SetActiveWidgetIndex(SafeIndex);
	}
}

void ULomoWidgetSwitcher::SetActiveWidgetIndex(int32 Index)
{
	// Deactivate old
	const int32 LastActiveIndex = GetActiveWidgetIndex();
	
	// Super
	Super::SetActiveWidgetIndex(Index);

	// if should deactivate previous
	if (LastActiveIndex != GetActiveWidgetIndex())
	{
		if (UWidget* LastActiveWidget = GetWidgetAtIndex(LastActiveIndex))
		{
			// Call interface on deactivated
			// if (LastActiveWidget->GetClass()->ImplementsInterface(UACCActivableWidget::StaticClass()))
			// {
			// 	IACCActivableWidget::Execute_OnDeactivated(LastActiveWidget);
			// }
		}
	}
	// if should active newer
	if (UWidget* NewActiveWidget = GetActiveWidget())
	{
		// Call interface on activated
		// if (NewActiveWidget->GetClass()->ImplementsInterface(UACCActivableWidget::StaticClass()))
		// {
		// 	IACCActivableWidget::Execute_OnActivated(NewActiveWidget);
		// }
	}
}

#if WITH_EDITOR
void ULomoWidgetSwitcher::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	SetSlotVisibility(GetActiveWidgetIndex());
}
#endif

void ULomoWidgetSwitcher::SynchronizeProperties()
{
	// Only super of UPanel
	UPanelWidget::SynchronizeProperties();

	SetSlotVisibility(GetActiveWidgetIndex());
}
