#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "ProjectJGameplayEffectContext.generated.h"

/**
 * 它在整个效果执行过程中传递，因此它是跟踪有关执行的瞬态信息的好地方； 每次Apply, 即便是层数GE， 这个Context也都是新的， 因此没法保存持久化数据
 */
USTRUCT(BlueprintType)
struct FProjectJGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	void SetDuration(int32 InDuration)
	{
		Duration = InDuration;
	}

	int32 GetDuration() const
	{
		return Duration;
	}

protected:
	int32 Duration = -1;
	
public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
 
	/** Creates a copy of this context, used to duplicate for later modifications */
	// virtual FProjectJGameplayEffectContext* Duplicate() const override
	// {
	// 	FProjectJGameplayEffectContext* NewContext = new FProjectJGameplayEffectContext();
	// 	*NewContext = *this;
	// 	// Todo: 基类中没有这行 复制Actors
	//  NewContext->AddActors(Actors);
	// 	if (GetHitResult())
	// 	{
	// 		// Does a deep copy of the hit result
	// 		NewContext->AddHitResult(*GetHitResult(), true);
	// 	}
	// 	return NewContext;
	// }
 
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

