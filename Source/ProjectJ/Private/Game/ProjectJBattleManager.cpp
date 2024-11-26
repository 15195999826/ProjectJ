// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJBattleManager.h"

#include "AbilitySystemComponent.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "Types/ProjectJCardAnimState.h"

// Sets default values
AProjectJBattleManager::AProjectJBattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectJBattleManager::BeginPlay()
{
	Super::BeginPlay();
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnWaitingAttack.AddUObject(this, &AProjectJBattleManager::OnWaitingAttack);
}

FProjectJExecEventRet AProjectJBattleManager::ExecuteEvent(const FProjectJBattleEventData& InEventData)
{
	FProjectJExecEventRet Ret;

	
	
	return Ret;
}

// Called every frame
void AProjectJBattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsRunningBattle)
	{
		return;
	}

	if (WaitSignals.Num() == 0)
	{
		switch (Stage)
		{
			case EProjectJBattleStage::EnterBattle:
				{
					ChangeStage(EProjectJBattleStage::RoundStart);
				}
				break;
			case EProjectJBattleStage::RoundStart:
				{
					if (BattleContext.OrderArray.Num() > 0)
					{
						ChangeStage(EProjectJBattleStage::CharacterPrepareAttack);
					}
					else
					{
						ChangeStage(EProjectJBattleStage::EndBattle);
					}
				}
				break;
			case EProjectJBattleStage::CharacterPrepareAttack:
				{
					ChangeStage(EProjectJBattleStage::CharacterStartAttack);
				}
				break;
			case EProjectJBattleStage::CharacterStartAttack:
				{
					ChangeStage(EProjectJBattleStage::CharacterDoAttack);
				}
				break;
			case EProjectJBattleStage::CharacterDoAttack:
				{
					ChangeStage(EProjectJBattleStage::CharacterEndAttack);
				}
				break;
			case EProjectJBattleStage::CharacterEndAttack:
				{
					if (BattleContext.AttackTimes == BattleContext.AttackTotalTimes)
					{
						// 轮到下一个角色准备攻击
						if (BattleContext.CurrentOrderIndex >= BattleContext.OrderArray.Num())
						{
							ChangeStage(EProjectJBattleStage::RoundEnd);
						}
						else
						{
							ChangeStage(EProjectJBattleStage::CharacterPrepareAttack);
						}
					}
					else
					{
						// 依然时当前角色攻击
						ChangeStage(EProjectJBattleStage::CharacterStartAttack);
					}
				}
				break;
			case EProjectJBattleStage::RoundEnd:
				{
					ChangeStage(EProjectJBattleStage::RoundStart);
				}
				break;
			case EProjectJBattleStage::EndBattle:
				break;
		}
	}
}

void AProjectJBattleManager::StartBattle()
{
	IsRunningBattle = true;
	BattleContext = FProjectJBattleContext();
	for (const auto& Tuple : BattleCharacterMap)
	{
		auto TeamID = static_cast<int32>(Tuple.Value->GetAbilitySystemComponent()->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
		if (TeamID == 0)
		{
			BattleContext.BottomTeam.Add(Tuple.Key);
		}
		else
		{
			BattleContext.TopTeam.Add(Tuple.Key);
		}
	}
}

void AProjectJBattleManager::ChangeStage(EProjectJBattleStage NewStage)
{
	if (NewStage == Stage)
	{
		UE_LOG(LogTemp, Warning, TEXT("相同状态切换，逻辑可能错误, Stage: %s"), *BattleStageToStr[Stage]);
		return;
	}

	if (WaitSignals.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("上一个阶段尚未完成, Stage: %s"), *BattleStageToStr[Stage]);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[BattleManager] Change Stage %s -> %s"), *BattleStageToStr[Stage], *BattleStageToStr[NewStage]);
	auto OldStage = Stage;
	Stage = NewStage;
	OnStageChange(OldStage, NewStage);
}

void AProjectJBattleManager::OnStageChange(EProjectJBattleStage OldStage, EProjectJBattleStage NewStage)
{
	// Leave Old And Enter New
	switch (OldStage) {
		case EProjectJBattleStage::EnterBattle:
			break;
		case EProjectJBattleStage::RoundStart:
			break;
		case EProjectJBattleStage::CharacterPrepareAttack:
			break;
		case EProjectJBattleStage::CharacterStartAttack:
			break;
		case EProjectJBattleStage::CharacterDoAttack:
			break;
		case EProjectJBattleStage::CharacterEndAttack:
			break;
		case EProjectJBattleStage::RoundEnd:
			break;
		case EProjectJBattleStage::EndBattle:
			break;

	}

	switch (NewStage) {
		case EProjectJBattleStage::EnterBattle:
			break;
		case EProjectJBattleStage::RoundStart:
			{
				RoundStart();
			}
			break;
		case EProjectJBattleStage::CharacterPrepareAttack:
			{
				// Todo: 确定攻击次数
				BattleContext.CurrentOrderIndex = BattleContext.CurrentOrderIndex + 1;
				BattleContext.AttackTimes = 0;
				BattleContext.AttackTotalTimes = 1;
				BattleContext.AttackTargets.Empty();
			}
			break;
		case EProjectJBattleStage::CharacterStartAttack:
			{
				BattleContext.AttackTimes = BattleContext.AttackTimes + 1;
				WaitSignals.Add(SignalSimplePending);
				// 1. 先播放当前攻击者StartAttack动画
				// 2. 等待动画播放完毕
				// 3. 抛出战斗事件， XXX 攻击前， 检查是否触发了其它卡牌的技能
				//   - 如果有技能动画， 要等待技能动画执行完毕
				auto CurrentCharacter = BattleCharacterMap[BattleContext.OrderArray[BattleContext.CurrentOrderIndex]];
				FProjectJCharacterAniData AniData;
				// 选择最近的目标
				auto SelfPosition = static_cast<int32>(CurrentCharacter->GetAbilitySystemComponent()->GetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute()));
				// 遍历敌方队伍，	Position相减，取绝对值，最小的就是最近的
				auto SelfTeam = static_cast<int32>(CurrentCharacter->GetAbilitySystemComponent()->GetNumericAttributeBase(UProjectJCharacterAttributeSet::GetTeamAttribute()));
				const auto& TargetTeam = SelfTeam == 0 ? BattleContext.TopTeam : BattleContext.BottomTeam;
				int32 MinDistance = INT32_MAX;
				int32 MinTargetID = 0;
				for (const auto& TargetID : TargetTeam)
				{
					auto TargetCharacter = BattleCharacterMap[TargetID];
					auto TargetPosition = static_cast<int32>(TargetCharacter->GetAbilitySystemComponent()->GetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute()));
					auto Distance = FMath::Abs(SelfPosition - TargetPosition);
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						MinTargetID = TargetID;
					}
				}

				BattleContext.AttackTargets.Add(MinTargetID);
				AniData.TargetCards.Add(BattleCharacterMap[MinTargetID].Get());
				
				CurrentCharacter->ChangeAnimState(EProjectJCardAnimState::AttackStart, AniData);
			}
			break;
		case EProjectJBattleStage::CharacterDoAttack:
			{
				WaitSignals.Add(SignalSimplePending);
				// 1. 当前卡牌播放DoAttack动画， 在ActionPoint执行造成伤害的功能
				// 2. 等待动画播放完毕
				// 3. 战斗中产生了, XXX受伤事件， XXX死亡，依次存入事件队列， 每次向一个角色发送， 如果该角色有技能，就先执行它的，再执行下一个人的。
				auto CurrentCharacter = BattleCharacterMap[BattleContext.OrderArray[BattleContext.CurrentOrderIndex]];
				FProjectJCharacterAniData AniData;
				AniData.TargetCards.Add(BattleCharacterMap[BattleContext.AttackTargets[0]].Get());
				
				CurrentCharacter->ChangeAnimState(EProjectJCardAnimState::DoAttack, AniData);
			}
			break;
		case EProjectJBattleStage::CharacterEndAttack:
			{
				// 1. 当前卡牌动画切换会Idle状态
				// 2. 等待动画播放完毕
				// 3. 抛出战斗事件， XXX攻击后， 检查是否触发了其它卡牌的技能
			}
			break;
		case EProjectJBattleStage::RoundEnd:
			{
				// 1. 回合结束事件
			}
			break;
		case EProjectJBattleStage::EndBattle:
			{
				
			}
			break;
	}
}

void AProjectJBattleManager::RoundStart()
{
	BattleContext.OrderArray.Empty();
	// Todo: 评价身法，决定队伍先手, 目前攻击顺序总是， 玩家0，1，2，3，敌人0，1，2，3
	for (const auto& CharacterID : BattleContext.BottomTeam)
	{
		const auto& Character = BattleCharacterMap[CharacterID];
		if (Character->IsDead())
		{
			continue;
		}
		BattleContext.OrderArray.Add(CharacterID);
	}

	for (const auto& CharacterID : BattleContext.TopTeam)
	{
		const auto& Character = BattleCharacterMap[CharacterID];
		if (Character->IsDead())
		{
			continue;
		}
		BattleContext.OrderArray.Add(CharacterID);
	}

	BattleContext.CurrentOrderIndex = -1;
	
	// Todo: 回合开始事件
}

void AProjectJBattleManager::OnWaitingAttack(int InCharacterID)
{
	check(InCharacterID == BattleContext.OrderArray[BattleContext.CurrentOrderIndex]);
	check(WaitSignals.Contains(SignalSimplePending));
	WaitSignals.Remove(SignalSimplePending);
}

