// Fill out your copyright notice in the Description page of Project Settings.


#include "LCCSpecieConfig.h"

bool ULCCSpecieConfig::IsSpecieTupleDisable(ELCCMaturity InMaturity, ELCCGender InGender,
	const TArray<FLCCSpecieTuple>& SpecieTuple)
{
	for (const FLCCSpecieTuple& Tuple : SpecieTuple)
	{
		if (Tuple.Maturity == InMaturity && Tuple.Gender == InGender)
		{
			return true;
		}
	}

	return false;
}
