// Copyright Ogopogo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ProjectJSimpleAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJSimpleAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FGameplayTag Tag;
};
