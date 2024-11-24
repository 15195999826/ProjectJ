// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJBattleManager.h"

#include "AbilitySystemComponent.h"
#include "Game/Card/ProjectJCharacter.h"

// Sets default values
AProjectJBattleManager::AProjectJBattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectJBattleManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectJBattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectJBattleManager::StartBattle()
{
	for (auto& Character : BattleCharacters)
	{
		// 使用Attack技能
		auto AttackHandle = Character->AttackAbilitySpecHandle;
		Character->GetAbilitySystemComponent()->TryActivateAbility(AttackHandle, false);
	}
}

