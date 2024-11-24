// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProjectJPlayerPawn.generated.h"

class ASpringArmCameraActor;

#define DEFINE_PLAYER_ATTRIBUTE(PropertyType, PropertyName) \
    protected: \
	PropertyType PropertyName; \
	public: \
	UPROPERTY(BlueprintAssignable) \
	FProjectJ##PropertyName##ChangeSignature On##PropertyName##Change; \
	FORCEINLINE PropertyType Get##PropertyName() const { return PropertyName; } \
	FORCEINLINE void Set##PropertyName(PropertyType NewVal) \
	{ \
	    if (PropertyName == NewVal) return; \
	    PropertyType OldVal = PropertyName; \
		PropertyName = NewVal; \
		On##PropertyName##Change.Broadcast(OldVal, NewVal); \
	}
UCLASS()
class PROJECTJ_API AProjectJPlayerPawn : public APawn
{
	GENERATED_BODY()
	

public:
	// Sets default values for this pawn's properties
	AProjectJPlayerPawn();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY()
	TWeakObjectPtr<ASpringArmCameraActor> MainCamera;

	// Define player attributes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProjectJGoldChangeSignature, int32, OldVal, int32, NewVal);
	DEFINE_PLAYER_ATTRIBUTE(int32, Gold)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProjectJUsingCharacterIDChangeSignature, int32, OldVal, int32, NewVal);
	DEFINE_PLAYER_ATTRIBUTE(int32, UsingCharacterID)
};
