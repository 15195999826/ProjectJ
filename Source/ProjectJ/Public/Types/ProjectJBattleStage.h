#pragma once

UENUM()
enum class EProjectJBattleStage : uint8
{
	EnterBattle,
	RoundStart,
	CharacterGetTurn, // 无事件，目前用于确定，当前玩家应该执行几次攻击循环
	CharacterStartAttack,
	CharacterDoAttack,
	CharacterEndAttack,
	CharacterEndTurn,
	RoundEnd,

	EndBattle,
};
