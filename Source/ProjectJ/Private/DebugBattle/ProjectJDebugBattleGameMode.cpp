// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugBattle/ProjectJDebugBattleGameMode.h"

#include "AbilitySystemComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJBattleManager.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "Types/Item/ProjectJItemBase.h"

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

	ContextSystem->BattleManager->BattleCharacters.Add(Character);
}

void AProjectJDebugBattleGameMode::EnterDebugBattle()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TMap<int32, int32> TeamCountMap = {
		{0, 0},
		{1, 0}
	};
	for (auto& TeamCharacter : ContextSystem->BattleManager->BattleCharacters)
	{
		auto TeamASC = TeamCharacter->GetAbilitySystemComponent();
		auto LocalTeamID = static_cast<int32>(TeamASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
		TeamCountMap[LocalTeamID]++;
	}
	
	for (auto& Character : ContextSystem->BattleManager->BattleCharacters)
	{
		auto ASC = Character->GetAbilitySystemComponent();
		auto TeamID = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
		auto Position = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetPositionAttribute()));
		
		
		Character->SetActorLocation(GetTeamPosition(TeamID, Position, TeamCountMap[TeamID]));
	}
}

FVector AProjectJDebugBattleGameMode::GetTeamPosition(int32 InTeamID, int32 InPosition, int32 InTotalCount)
{
	// 始终保持队伍显示居中
	const FVector& TeamCenterPos = InTeamID == 0 ? TopTeamCenterPos : BottomTeamCenterPos;

	// Calculate the offset for each character
	float Offset = TeamOffset.X * (InPosition - (InTotalCount - 1) / 2.0f);

	// Return the calculated position
	return TeamCenterPos + FVector(Offset, 0, 0);
}