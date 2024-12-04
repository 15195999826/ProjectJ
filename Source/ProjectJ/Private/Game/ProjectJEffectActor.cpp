// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJEffectActor.h"

#include "NiagaraComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Particles/ParticleSystemComponent.h"

class UProjectJContextSystem;
// Sets default values
AProjectJEffectActor::AProjectJEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CascadeComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CascadeComponent"));
	CascadeComponent->SetupAttachment(SceneRoot);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AProjectJEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectJEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsRunning)
	{
		return;
	}

	Timer += DeltaTime;
	if (Timer >= Duration)
	{
		IsRunning = false;
		
		OnEffectEnd();
	}
}

void AProjectJEffectActor::StartEffect(bool Reset)
{
	if (CascadeComponent->Template)
	{
		CascadeComponent->Activate(Reset);
	}

	if (NiagaraComponent->GetFXSystemAsset())
	{
		NiagaraComponent->Activate(Reset);
	}

	Timer = 0.f;
	IsRunning = true;
}

void AProjectJEffectActor::OnEffectEnd_Implementation()
{
	CascadeComponent->Deactivate();
	NiagaraComponent->Deactivate();
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->RecycleEffectActor(this);
}
