// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameState/ProjectJSpawnManagerComponent.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "ProjectJ/ProjectJLogChannels.h"


// Sets default values for this component's properties
UProjectJSpawnManagerComponent::UProjectJSpawnManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UProjectJSpawnManagerComponent::InitializeComponent()
{
	UE_LOG(LogProjectJ, Log, TEXT("UTheOneSpawnManagerComponent::InitializeComponent"));
	Super::InitializeComponent();
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	// UWorld* World = GetWorld();
	// World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));
	//
	// for (TActorIterator<APlayerStart> It(World); It; ++It)
	// {
	// 	if (APlayerStart* PlayerStart = *It)
	// 	{
	// 		CachedPlayerStarts.Add(PlayerStart);
	// 	}
	// }
}

bool UProjectJSpawnManagerComponent::TryFindPlayerStart(const FName& Tag, AActor*& OutStart)
{
	// for (auto PlayerStart : CachedPlayerStarts)
	// {
	// 	if (!PlayerStart.IsValid())
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	if (PlayerStart->PlayerStartTag == Tag)
	// 	{
	// 		OutStart = PlayerStart.Get();
	// 		return true;
	// 	}
	// }

	return false;
}

void UProjectJSpawnManagerComponent::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld())
	{
		for (AActor* Actor : InLevel->Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				// ensure(!CachedPlayerStarts.Contains(PlayerStart));
				// CachedPlayerStarts.Add(PlayerStart);
			}
		}
	} else
	{
		UE_LOG(LogProjectJ, Warning, TEXT("UTheOneSpawnManagerComponent::OnLevelAdded: InWorld != GetWorld(), InWorld: %s, GetWorld(): %s"), *InWorld->GetName(), *GetWorld()->GetName());
	}
}

void UProjectJSpawnManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	// if (APlayerStart* PlayerStart = Cast<APlayerStart>(SpawnedActor))
	// {
	// 	CachedPlayerStarts.Add(PlayerStart);
	// }
}

// Called when the game starts
void UProjectJSpawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}