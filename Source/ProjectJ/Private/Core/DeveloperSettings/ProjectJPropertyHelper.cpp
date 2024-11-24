// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"

#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"

TArray<FName> UProjectJPropertyHelper::GetStarOneCharacters()
{
	TArray<FName> Result;

	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetStarTwoCharacters()
{
	TArray<FName> Result;

	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetStarThreeCharacters()
{
	TArray<FName> Result;
	
	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetStarFourCharacters()
{
	TArray<FName> Result;
	
	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetStarFiveCharacters()
{
	TArray<FName> Result;
	
	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetStarSixCharacters()
{
	TArray<FName> Result;
	
	return Result;
}

TArray<FName> UProjectJPropertyHelper::GetAbilityRowNames()
{
	TArray<FName> Result;
	auto AbilityTable = GetDefault<UProjectJDataTableSettings>()->AbilityTable.LoadSynchronous();
	if (AbilityTable)
	{
		for (const auto& Row : AbilityTable->GetRowMap())
		{
			Result.Add(Row.Key);
		}
	}
	return Result;
}