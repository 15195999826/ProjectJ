// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJLuaInstanceType.h"
#include "ProjectJLuaExecutor.generated.h"

UCLASS()
class PROJECTJ_API AProjectJLuaExecutor : public AActor, public IUnLuaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLuaExecutor();

	virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("ProjectJLuaExecutor");
	}

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void FirstTimeEnterLevel(const FName& RowName, const FName& ScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void EnterLevel(const FName& RowName);
	
protected:
	// UFUNCTION(BlueprintImplementableEvent)
	
};
