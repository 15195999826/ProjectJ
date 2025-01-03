// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/CardType.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJCardExecuteHelper.generated.h"

class UProjectJContextSystem;
class AProjectJCardBase;
/**
 * 副本帮助类
 * 生成卡牌会添加到GameContext， 移除卡牌会从GameContext移除
 */
UCLASS()
class PROJECTJ_API UProjectJCardExecuteHelper : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 给与卡牌显示条件
	UFUNCTION()
	void GiveCardDisplayCondition(int32 InCardID, const FName& InConditionTag, const FString& InConditionValue);
	
	// 在随机位置生成
	UFUNCTION()
	int32 SpawnCharacter(const FName& InRowName);
	// 在指定位置生成
	UFUNCTION()
	int32 SpawnCharacterAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout = true);
	/**
	 * 目前总是从执行区生成的， 因此固定向执行区右上45°， （300，300）的位置生成
	 * @return int32 The ID of the spawned card.
	 */
	UFUNCTION()
	int32 PopupCharacter(const FName& InRowName);

	UFUNCTION()
	int32 SpawnLandmark(const FName& InRowName);
	UFUNCTION()
	int32 SpawnLandmarkAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout = true);
	UFUNCTION()
	int32 PopupLandmark(const FName& InRowName);

	UFUNCTION()
	int32 SpawnUtility(const FName& InRowName);
	UFUNCTION()
	int32 SpawnUtilityAtLocation(const FName& InRowName, const FVector& InLocation, bool Relayout = true);
	UFUNCTION()
	int32 PopupUtility(const FName& InRowName);

	UFUNCTION()
	int32 SpawnItem(const EProjectJItemType InItemType, const FName& InRowName);
	UFUNCTION()
	int32 SpawnItemAtLocation(const EProjectJItemType InItemType, const FName& InRowName, const FVector& InLocation, bool Relayout = true);
	UFUNCTION()
	int32 PopupItem(const EProjectJItemType InItemType, const FName& InRowName);
	
	UFUNCTION()
	void DestroyCard(int32 InCardID) const;

	UFUNCTION()
	int32 Roll(const FName& InTagName) const;

	UFUNCTION()
	void ReSpawnSpell(int32 ID);

	UFUNCTION()
	void OnSpellToCard(const FName& InSpellTag, int32 TargetID, int32 RollResult);

	// UFUNCTION()
	// void Speak(int32 ID, TArray<FText> InText);

	UFUNCTION()
	void DefaultActionToSpell(const FName& InSpellTag, int32 ID);
	
	int32 SpawnSpell(const FName& InSpellTag, const FVector& InLocation, UProjectJContextSystem* InContextSystem);
	
private:
	AProjectJCardBase* IntervalSpawnNewCard(UProjectJContextSystem* InContextSystem, const FVector& InLocation, const FName& InRowName, EProjectJCardType InType, EProjectJItemType InItemType = EProjectJItemType::None);
	void IntervalPopupNewCard(AProjectJCardBase* InCard, UProjectJContextSystem* InContextSystem);
	static void IntervalScaleSpawnNewCard(AProjectJCardBase* InCard, const UProjectJContextSystem* InContextSystem, bool Relayout);

};
