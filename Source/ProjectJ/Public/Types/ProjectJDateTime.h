#pragma once
#include "CoreMinimal.h"
#include "ProjectJDateTime.generated.h"

USTRUCT(BlueprintType)
struct FProjectJDateTime
{
	GENERATED_BODY()

	// 新游戏从第一年，第一天，6点开始
	FProjectJDateTime() : Year(1),Day(1), Hour(6), Minute(0)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	int32 Year;

	UPROPERTY(BlueprintReadOnly)
	int32 Day;

	UPROPERTY(BlueprintReadOnly)
	int32 Hour;

	UPROPERTY(BlueprintReadOnly)
	int32 Minute;

	// Helper function to step the minute
	void StepMinute()
	{
		Minute++;
		if (Minute >= 60)
		{
			Minute = 0;
			Hour++;
			if (Hour >= 24)
			{
				Hour = 0;
				Day++;
				// 每年按照360天计算
				if (Day >= 360)
				{
					Day = 0;
					Year++;
				}
			}
		}
	}
};
