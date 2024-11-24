// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/System/ProjectJContextSystem.h"

#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Game/ProjectJCardBackendSystem.h"
#include "Game/ProjectJNavPointActor.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/Card/ProjectJLandmark.h"
#include "Game/Card/ProjectJSpell.h"

AProjectJSpell* UProjectJContextSystem::GetSpell()
{
	AProjectJSpell* Ret;
	if (SpellPool.Num() > 0)
	{
		Ret = SpellPool.Pop();
	}
	else
	{
		auto SpellClass = GetDefault<UProjectJGeneralSettings>()->SpellClass;
		Ret = GetWorld()->SpawnActor<AProjectJSpell>(SpellClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingSpells.Add(Ret->ID, Ret);
	return Ret;
}

AProjectJCharacter* UProjectJContextSystem::GetCharacter()
{
	AProjectJCharacter* Ret;
	if (CharacterPool.Num() > 0)
	{
		Ret = CharacterPool.Pop();
	}
	else
	{
		auto CharacterClass = GetDefault<UProjectJGeneralSettings>()->CharacterClass;
		Ret = GetWorld()->SpawnActor<AProjectJCharacter>(CharacterClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingCharacters.Add(Ret->ID, Ret);
#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingCharacters"));
#endif
	return Ret;
}

AProjectJLandmark* UProjectJContextSystem::GetLandMark()
{
	AProjectJLandmark* Ret;
	if (LandmarkPool.Num() > 0)
	{
		Ret = LandmarkPool.Pop();
	}
	else
	{
		auto LandmarkClass = GetDefault<UProjectJGeneralSettings>()->LandmarkClass;
		Ret = GetWorld()->SpawnActor<AProjectJLandmark>(LandmarkClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingLandmarks.Add(Ret->ID, Ret);
#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingLandmarks"));
#endif
	return Ret;
}

AProjectJNavPointActor* UProjectJContextSystem::GetNavPoint()
{
	AProjectJNavPointActor* Ret;
	if (NavPointPool.Num() > 0)
	{
		Ret = NavPointPool.Pop();
	}
	else
	{
		auto NavPointClass = GetDefault<UProjectJGeneralSettings>()->NavPointClass;
		Ret = GetWorld()->SpawnActor<AProjectJNavPointActor>(NavPointClass);
	}
	check(Ret);
	
	Ret->ID = GID++;
	GeneralOnGet(Ret);
	UsingNavPoints.Add(Ret->ID, Ret);
#if WITH_EDITOR
	Ret->SetFolderPath(TEXT("UsingNavPoints"));
#endif
	return Ret;
}

AProjectJSpell* UProjectJContextSystem::CreateSpell(const FName& Config)
{
	AProjectJSpell* Spell = GetSpell();
	IProjectJCardInterface::Execute_BindConfig(Spell, Config);
	return Spell;
}

AProjectJCharacter* UProjectJContextSystem::CreateCharacter(const FName& Config)
{
	AProjectJCharacter* Character = GetCharacter();
	IProjectJCardInterface::Execute_BindConfig(Character, Config);
	
	// Todo: 在ItemSystem注册装备槽位
	// auto ItemSystem = GetWorld()->GetSubsystem<UProjectJCardBackendSystem>();
	// ItemSystem->RegisterCharacterLogicSlots(Character->ID);
	return Character;
}

AProjectJLandmark* UProjectJContextSystem::CreateLandMark(const FName& Config)
{
	AProjectJLandmark* LandMark = GetLandMark();
	IProjectJCardInterface::Execute_BindConfig(LandMark, Config);
	return LandMark;
}

AProjectJNavPointActor* UProjectJContextSystem::CreateNavPoint(const FProjectJNavPoint& Config)
{
	AProjectJNavPointActor* NavPoint = GetNavPoint();
	NavPoint->BindConfig(Config);
	return NavPoint;
}

void UProjectJContextSystem::RecycleByID(int32 InID)
{
	if (UsingSpells.Contains(InID))
	{
		RecycleSpell(UsingSpells[InID].Get());
	}
	else if (UsingCharacters.Contains(InID))
	{
		RecycleCharacter(UsingCharacters[InID].Get());
	}
	else if (UsingLandmarks.Contains(InID))
	{
		RecycleLandMark(UsingLandmarks[InID].Get());
	}
	else if (UsingNavPoints.Contains(InID))
	{
		RecycleNavPoint(UsingNavPoints[InID].Get());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RecycleByID: Can't find ID: %d"), InID);
	}
}

void UProjectJContextSystem::RecycleNavPoint(AProjectJNavPointActor* NavPoint)
{
	UsingNavPoints.Remove(NavPoint->ID);
	NavPointPool.Add(NavPoint);
	GeneralOnRecycle(NavPoint);
#if WITH_EDITOR
	NavPoint->SetFolderPath(TEXT("NavPointPool"));
#endif
}

void UProjectJContextSystem::RecycleSpell(AProjectJSpell* Spell)
{
	// Detach
	if (Spell->GetAttachParentActor())
	{
		Spell->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	UsingSpells.Remove(Spell->ID);
	SpellPool.Add(Spell);
	GeneralOnRecycle(Spell);
#if WITH_EDITOR
	Spell->SetFolderPath(TEXT("SpellPool"));
#endif
}

void UProjectJContextSystem::RecycleCharacter(AProjectJCharacter* Character)
{
	auto ItemSystem = GetWorld()->GetSubsystem<UProjectJCardBackendSystem>();
	ItemSystem->RemoveCharacterLogicSlots(Character->ID);
	UsingCharacters.Remove(Character->ID);
	CharacterPool.Add(Character);
	GeneralOnRecycle(Character);
#if WITH_EDITOR
	Character->SetFolderPath(TEXT("CharacterPool"));
#endif
}

void UProjectJContextSystem::RecycleLandMark(AProjectJLandmark* LandMark)
{
	UsingLandmarks.Remove(LandMark->ID);
	LandmarkPool.Add(LandMark);
	GeneralOnRecycle(LandMark);
#if WITH_EDITOR
	LandMark->SetFolderPath(TEXT("LandmarkPool"));
#endif
}

void UProjectJContextSystem::GeneralOnGet(AActor* InActor)
{
	InActor->SetActorEnableCollision(true);
}

void UProjectJContextSystem::GeneralOnRecycle(AActor* InActor)
{
	InActor->SetActorEnableCollision(false);
	InActor->SetActorLocation(HiddenLocation);
}
