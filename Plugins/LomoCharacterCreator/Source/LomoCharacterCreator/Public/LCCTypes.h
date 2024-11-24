#pragma once
#include "CoreMinimal.h"
#include "LCCTypes.generated.h"

UENUM(BlueprintType)
enum class ELCCGender : uint8
{
	None,
	Male,
	Female
};

UENUM(BlueprintType)
enum class ELCCMaturity : uint8
{
	None,
	Baby UMETA(DisplayName = "Baby"),
	Kid UMETA(DisplayName = "Kid"),
	Teen UMETA(DisplayName = "青年"),
	Adult UMETA(DisplayName = "成年"),
	Elder UMETA(DisplayName = "老年"),
	Custom UMETA(DisplayName = "Custom")
};

USTRUCT(BlueprintType)
struct LOMOCHARACTERCREATOR_API FLomoGroupBtnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntPayload = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Label;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CustomInfo;
};

