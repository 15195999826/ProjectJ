// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJSpell.h"

#include "Components/TextRenderComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "ProjectJ/ProjectJGlobal.h"

void AProjectJSpell::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	SpellTag = InRowName;
	static TMap<FName, FText> SpellNameMap = {
		{ProjectJGlobal::GuanCha, NSLOCTEXT("Spell", "GuanCha", "观察")},
		{ProjectJGlobal::YinBi, NSLOCTEXT("Spell", "YinBi", "隐蔽")},
		{ProjectJGlobal::TouXi, NSLOCTEXT("Spell", "TouXi", "偷袭")},
		{ProjectJGlobal::TouQie, NSLOCTEXT("Spell", "TouQie", "偷窃")},
	};
	if (SpellNameMap.Contains(InRowName))
	{
		Title->SetText(SpellNameMap[InRowName]);
	}
	else
	{
		Title->SetText(FText::FromString(InRowName.ToString()));
	}
}

FName AProjectJSpell::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}

// void AProjectJSpell::OnDragStart()
// {
// 	Super::OnDragStart();
// 	// DragStartTransform = GetActorTransform().GetRelativeTransform(GetAttachParentActor()->GetActorTransform());
// 	// DragStartTransform = GetActorTransform();
// }

// void AProjectJSpell::OnDrop(float InDuration)
// {
// 	// Todo: 临时处理方案
// 	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
// 	check(ContextSystem->WeakFocusCard.IsValid());
// 	auto FocusCard = ContextSystem->WeakFocusCard.Get();
// 	auto FocusCardLocation = FocusCard->GetActorLocation();
// 	FocusCard->SetActorLocation(FVector(FocusCardLocation.X, FocusCardLocation.Y, 15.f));
// 	
// 	SetActorLocation(FVector(FocusCardLocation.X + 70.f, FocusCardLocation.Y, 1.f));
// 	SetActorRotation(FocusCard->GetActorRotation());
// 	
// 	SetActorEnableCollision(true);
// 	FocusCard->OnLoseSpellFocus();
// 	ContextSystem->WeakFocusCard = nullptr;
//
// 	// Todo: 临时处理
// 	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
// 	if (ConfigRowName == TEXT("观察"))
// 	{
// 		EventSystem->OnObserveCard.Broadcast(FocusCard);
// 	}
// 	else if (ConfigRowName == TEXT("检查"))
// 	{
// 		EventSystem->OnCheckCard.Broadcast(FocusCard);
// 	}
// 	else if (ConfigRowName == TEXT("躲藏"))
// 	{
// 		EventSystem->OnHideAtCard.Broadcast(FocusCard);
// 	}
// 	else if (ConfigRowName == TEXT("偷袭"))
// 	{
// 		EventSystem->OnAmbushCard.Broadcast(FocusCard);
// 	}
// }
//
// void AProjectJSpell::OnCancelDrag()
// {
// 	SetActorEnableCollision(true);
// 	// SetActorTransform(DragStartTransform * GetAttachParentActor()->GetActorTransform());
// 	SetActorTransform(DragStartTransform);
// }

