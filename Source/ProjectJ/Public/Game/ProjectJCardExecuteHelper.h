// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/CardType.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJCardExecuteHelper.generated.h"

class UProjectJContextSystem;
class AProjectJCardBase;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJCardExecuteHelper : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Lua侧调用, 目前总是从执行区生成的， 因此固定向执行区右上45°， （300，300）的位置生成
	 * @return int32 The ID of the spawned card.
	 */
	UFUNCTION()
	int32 SpawnNewCard(const FName& InRowName, EProjectJCardType InType, EProjectJItemType InItemType = EProjectJItemType::None);

	UFUNCTION()
	void DestroyCard(int32 InCardID);
};
