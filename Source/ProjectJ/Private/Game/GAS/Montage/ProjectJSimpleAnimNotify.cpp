// Copyright Ogopogo


#include "Game/GAS/Montage/ProjectJSimpleAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Game/GAS/ProjectJAbilitySystemComponent.h"

void UProjectJSimpleAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	bool isRunningInEditorPreview = MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview;
	AActor* Owner = MeshComp->GetOwner();

	if (isRunningInEditorPreview)
	{
		return;
	}
	
	auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (ASC)
	{
		auto PanguASC = Cast<UProjectJAbilitySystemComponent>(ASC);
		if (PanguASC)
		{
			PanguASC->HandleMontagePostEvent(Tag, FProjectJMontageEventData()); 
		}
	}
}
