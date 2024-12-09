#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "ProjectJGameplayEffectContext.generated.h"

// 完全不考虑网络情况下的序列化
USTRUCT(BlueprintType)
struct FProjectJGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	TMap<int32, int32> GetStackCountAtRoundMap() const { return StackCountAtRound; }

	void AddStackCountAtRound(int32 Round, int32 StackCount)
	{
		if (StackCountAtRound.Contains(Round))
		{
			StackCountAtRound[Round] += StackCount;
		}
		else
		{
			StackCountAtRound.Add(Round, StackCount);
		}
	}
protected:
	UPROPERTY(NotReplicated)
	TMap<int32, int32> StackCountAtRound; // 在 Key 回合 获得了 Value 层
 
public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
 
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FProjectJGameplayEffectContext* Duplicate() const override
	{
		FProjectJGameplayEffectContext* NewContext = new FProjectJGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
 
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
};
 
template <>
struct TStructOpsTypeTraits<FProjectJGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FProjectJGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};

