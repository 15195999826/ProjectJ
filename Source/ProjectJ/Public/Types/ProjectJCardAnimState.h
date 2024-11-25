#pragma once

UENUM(BlueprintType)
enum class EProjectJCardAnimState : uint8
{
	Idle,
	AttackStart,
	DoAttack,
	Death
};
