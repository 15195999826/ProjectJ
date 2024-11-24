#pragma once

#include "CoreMinimal.h"

#include "ProjectJLuaFunction.generated.h"

USTRUCT()
struct FProjectJLuaFunction
{
	GENERATED_BODY()
	
	FProjectJLuaFunction(){}

	FString FunctionName;

	TArray<FString> Parameters;

	FString Comment;

	FString DefaultContent;
};

UENUM()
enum class EProjectJCompareResult : uint8
{
	Match,
	NotMatch,
	Missing
};
