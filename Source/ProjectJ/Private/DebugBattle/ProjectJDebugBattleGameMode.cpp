// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugBattle/ProjectJDebugBattleGameMode.h"

#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJBattleManager.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "Types/Item/ProjectJItemBase.h"
#include "Game/ProjectJLevelSettingActor.h"

void AProjectJDebugBattleGameMode::BeginPlay()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	// 查询BattleManager
	for (TActorIterator<AProjectJBattleManager> It(GetWorld()); It; ++It)
	{
		ContextSystem->BattleManager = *It;
		break;
	}

	for (TActorIterator<AProjectJLevelSettingActor> It(GetWorld()); It; ++It)
	{
		auto LevelSetting = *It;
		ContextSystem->LevelSettingActor = LevelSetting;
		break;
	}
	
	Super::BeginPlay();
}

void AProjectJDebugBattleGameMode::RegisterTeamCharacter(int32 TeamID, const FString& InPosition, const FString& InCharacterRow,
                                                         const FString& InWeaponRow, const FString& InArmorRow)
{
	// 生成对应的角色卡
	// 生成Character
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto Character = ContextSystem->CreateCharacter(FName(*InCharacterRow));
	Character->CanDrag = false;
	// 装备武器和护甲
	UProjectJGameBPFL::Equip(Character, FName(*InWeaponRow), EProjectJItemType::Weapon);
	UProjectJGameBPFL::Equip(Character, FName(*InArmorRow), EProjectJItemType::Armor);

	Character->GetAbilitySystemComponent()->SetNumericAttributeBase(UProjectJCharacterAttributeSet::GetTeamAttribute(), TeamID);
	auto Position  = FCString::Atoi(*InPosition);
	Character->GetAbilitySystemComponent()->SetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute(), Position);

	ContextSystem->BattleManager->BattleCharacterMap.Add(Character->ID, Character);
}

void AProjectJDebugBattleGameMode::EnterDebugBattle()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TMap<int32, int32> TeamCountMap = {
		{0, 0},
		{1, 0}
	};
	for (auto& Tuple : ContextSystem->BattleManager->BattleCharacterMap)
	{
		auto TeamASC = Tuple.Value->GetAbilitySystemComponent();
		auto LocalTeamID = static_cast<int32>(TeamASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
		TeamCountMap[LocalTeamID]++;
	}
	
	for (auto& Tuple : ContextSystem->BattleManager->BattleCharacterMap)
	{
		auto ASC = Tuple.Value->GetAbilitySystemComponent();
		auto TeamID = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
		auto Position = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetPositionAttribute()));

		auto Location = ContextSystem->BattleManager->GetTeamPosition(TeamID, Position, TeamCountMap[TeamID]);
		UE_LOG(LogTemp, Log, TEXT("TeamID: %d, Position: %d, TeamCount: %d, Location: %s"), TeamID, Position, TeamCountMap[TeamID], *Location.ToString());
		
		Tuple.Value->SetActorLocation(Location);
	}
}