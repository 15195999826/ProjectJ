#include "Game/GAS/ProjectJGameplayEffectContext.h"

bool FProjectJGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	return FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);
}
