// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJBattleManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Core/ProjectJBlueprintFunctionLibrary.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/ProjectJLevelSettingActor.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Types/ProjectJCardAnimState.h"
#include "Types/Item/ProjectJEquipmentConfig.h"

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
	EventSystem->OnAttackHit.AddUObject(this, &AProjectJBattleManager::OnAttackHit);
	EventSystem->AfterAttackHit.AddUObject(this, &AProjectJBattleManager::AfterAttackHit);
	EventSystem->OnIdleReturnToPosition.AddUObject(this, &AProjectJBattleManager::OnIdleReturnToPosition);
}

FProjectJExecEventRet AProjectJBattleManager::ExecuteEvent(const FProjectJBattleEventData& InEventData)
{
	FProjectJExecEventRet Ret;

	
	
	return Ret;
}

FVector AProjectJBattleManager::GetTeamPosition(int32 InTeamID, int32 InPosition, int32 InTotalCount)
{
	auto LevelSettingActor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	// 始终保持队伍显示居中
	const FVector& TeamCenterPos = InTeamID == 1 ? LevelSettingActor->BattleAreaSettings.TopTeamCenterPos : LevelSettingActor->BattleAreaSettings.BottomTeamCenterPos;

	// Calculate the offset for each character
	float Offset = LevelSettingActor->BattleAreaSettings.TeamOffsetY * (InPosition - (InTotalCount - 1) / 2.0f);

	// Return the calculated position
	return TeamCenterPos + FVector(0, Offset, 0);
}

bool AProjectJBattleManager::IsBattleEnd()
{
	// 检查是否有一方全部死亡
	bool TopHasAlive = false;
	bool BottomHasAlive = false;
	for (const auto& Tuple : BattleCharacterMap)
	{
		if (!Tuple.Value->IsDead())
		{
			if (Tuple.Value->IsAtTopTeam())
			{
				TopHasAlive = true;
			}
			else
			{
				BottomHasAlive = true;
			}
		}
	}

	if (!TopHasAlive || !BottomHasAlive)
	{
		return true;
	}

	return false;
}

TWeakObjectPtr<AProjectJCharacter> AProjectJBattleManager::GetCurrentCharacter()
{
	return BattleContext.CurrentOrderIndex < BattleContext.OrderArray.Num() ? BattleCharacterMap[BattleContext.OrderArray[BattleContext.CurrentOrderIndex]] : nullptr;
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
						ChangeStage(EProjectJBattleStage::CharacterGetTurn);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("没有角色参与战斗，战斗结束"));
						ChangeStage(EProjectJBattleStage::EndBattle);
					}
				}
				break;
			case EProjectJBattleStage::CharacterGetTurn:
				{
					auto CurrentCharacter = GetCurrentCharacter();
					if (CurrentCharacter->IsDead())
					{
						ChangeStage(EProjectJBattleStage::CharacterEndTurn);
					}
					else
					{
						ChangeStage(EProjectJBattleStage::CharacterStartAttack);
					}
					
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
						ChangeStage(EProjectJBattleStage::CharacterEndTurn);
					}
					else
					{
						// 依然时当前角色攻击
						ChangeStage(EProjectJBattleStage::CharacterStartAttack);
					}
				}
				break;
			case EProjectJBattleStage::CharacterEndTurn:
				{
					// 检测战斗是否结束
					if (IsBattleEnd())
					{
						ChangeStage(EProjectJBattleStage::EndBattle);
					}
					else
					{
						// 轮到下一个角色准备攻击
						if (BattleContext.CurrentOrderIndex >= BattleContext.OrderArray.Num() - 1)
						{
							ChangeStage(EProjectJBattleStage::RoundEnd);
						}
						else
						{
							ChangeStage(EProjectJBattleStage::CharacterGetTurn);
						}
					}
				}
				break;
			case EProjectJBattleStage::RoundEnd:
				{
					if (IsBattleEnd())
					{
						ChangeStage(EProjectJBattleStage::EndBattle);
					}
					else
					{
						ChangeStage(EProjectJBattleStage::RoundStart);
					}
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
	TeamFillData = FProjectJTeamFillData();
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TeamFillData.Duration =ContextSystem->LevelSettingActor->ProgramAnimationSettings.TeamFillDuration;
	
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
		case EProjectJBattleStage::CharacterGetTurn:
			break;
		case EProjectJBattleStage::CharacterStartAttack:
			break;
		case EProjectJBattleStage::CharacterDoAttack:
			break;
		case EProjectJBattleStage::CharacterEndAttack:
			break;
		case EProjectJBattleStage::CharacterEndTurn:
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
		case EProjectJBattleStage::CharacterGetTurn:
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
				// 1. 先播放当前攻击者StartAttack动画
				// 2. 等待动画播放完毕
				// 3. 抛出战斗事件， XXX 攻击前， 检查是否触发了其它卡牌的技能
				//   - 如果有技能动画， 要等待技能动画执行完毕
				auto CurrentCharacter = GetCurrentCharacter();
				auto WeaponConfig = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<
					FProjectJWeaponConfig>(CurrentCharacter->TempWeaponRowName, TEXT("ChangeAnimState"));

				auto SelfTeam = static_cast<int32>(CurrentCharacter->GetAbilitySystemComponent()->GetNumericAttributeBase(UProjectJCharacterAttributeSet::GetTeamAttribute()));
				TArray<int32> PossibleTargets;
				switch (WeaponConfig->AttackAbility.TargetTeam) {
					case EProjectJTargetTeam::Teammate:
						PossibleTargets = SelfTeam == 0 ? BattleContext.BottomTeam : BattleContext.TopTeam;
						break;
					case EProjectJTargetTeam::Enemy:
						PossibleTargets = SelfTeam == 0 ? BattleContext.TopTeam : BattleContext.BottomTeam;
						break;
					case EProjectJTargetTeam::Any:
						{
							// 合并TopTeam和BottomTeam
							PossibleTargets.Append(BattleContext.TopTeam);
							PossibleTargets.Append(BattleContext.BottomTeam);
						}
						break;
				}

				// 如果目标已经死亡， 移除
				for (int32 i = PossibleTargets.Num() - 1; i >= 0; i--)
				{
					auto TargetCharacter = BattleCharacterMap[PossibleTargets[i]].Get();
					if (TargetCharacter->IsDead())
					{
						PossibleTargets.RemoveAt(i);
					}
				}
				
				switch (WeaponConfig->AttackAbility.AttackRange) {
					case EProjectJAttackRange::Closet:
						{
							// 选择最近的目标
							auto SelfPosition = static_cast<int32>(CurrentCharacter->GetAbilitySystemComponent()->GetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute()));
							// 遍历敌方队伍，	Position相减，取绝对值，最小的就是最近的
							
							int32 MinDistance = INT32_MAX;
							int32 MinTargetID = 0;
							for (const auto& TargetID : PossibleTargets)
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
						}
						break;
					case EProjectJAttackRange::Front:
						break;
					case EProjectJAttackRange::Back:
						break;
					case EProjectJAttackRange::AOE:
						{
							// 目标为全部PossibleTargets
							BattleContext.AttackTargets = PossibleTargets;
						}
						break;
					case EProjectJAttackRange::RandomOne:
						break;
				}

				check(BattleContext.AttackTargets.Num() > 0);
				
				BattleContext.AttackTimes = BattleContext.AttackTimes + 1;
				WaitSignals.Add(SignalSimplePending);
				
				FProjectJCharacterAniData AniData;
				switch (WeaponConfig->AttackAbility.AttackRange) {
					case EProjectJAttackRange::Closet:
						{
							auto TargetCharacter = BattleCharacterMap[BattleContext.AttackTargets[0]].Get();
							AniData.LocationPayload = TargetCharacter->GetActorLocation();
							AniData.TargetCards.Add(TargetCharacter);
						}
						break;
					case EProjectJAttackRange::Front:
						break;
					case EProjectJAttackRange::Back:
						break;
					case EProjectJAttackRange::AOE:
						{
							AniData.LocationPayload = CurrentCharacter->GetActorLocation();
							for (const auto& TargetID : BattleContext.AttackTargets)
							{
								AniData.TargetCards.Add(BattleCharacterMap[TargetID].Get());
							}
						}
						break;
					case EProjectJAttackRange::RandomOne:
						break;
				}
				
				
				CurrentCharacter->ChangeAnimState(EProjectJCardAnimState::AttackStart, AniData);
			}
			break;
		case EProjectJBattleStage::CharacterDoAttack:
			{
				WaitSignals.Add(SignalSimplePending);
				// 1. 当前卡牌播放DoAttack动画， 在ActionPoint执行造成伤害的功能
				// 2. 等待动画播放完毕
				// 3. 战斗中产生了, XXX受伤事件， XXX死亡，依次存入事件队列， 每次向一个角色发送， 如果该角色有技能，就先执行它的，再执行下一个人的。
				auto CurrentCharacter = GetCurrentCharacter();
				FProjectJCharacterAniData AniData;
				for (const auto& TargetID : BattleContext.AttackTargets)
				{
					AniData.TargetCards.Add(BattleCharacterMap[TargetID].Get());
				}
				
				CurrentCharacter->ChangeAnimState(EProjectJCardAnimState::DoAttack, AniData);
			}
			break;
		case EProjectJBattleStage::CharacterEndAttack:
			{
				WaitSignals.Add(SignalSimplePending);
				// 1. 当前卡牌动画切换会Idle状态
				// 2. 等待动画播放完毕
				// 3. 抛出战斗事件， XXX攻击后， 检查是否触发了其它卡牌的技能
				auto CurrentCharacter = GetCurrentCharacter();
				auto ASC = CurrentCharacter->GetAbilitySystemComponent();
				auto TeamID = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetTeamAttribute()));
				auto Position = static_cast<int32>(ASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetPositionAttribute()));
				int32 AliveCount = 0;
				for (const auto& CharacterID : TeamID == 0 ? BattleContext.BottomTeam : BattleContext.TopTeam)
				{
					if (!BattleCharacterMap[CharacterID]->IsDead())
					{
						AliveCount++;
					}
				}
				FProjectJCharacterAniData AniData;
				// 返回发起攻击的位置
				AniData.LocationPayload = GetTeamPosition(TeamID, Position, AliveCount);
				CurrentCharacter->ChangeAnimState(EProjectJCardAnimState::Idle, AniData);
			}
			break;
		case EProjectJBattleStage::CharacterEndTurn:
			{
				// Todo: 抛出行动结束事件， 如果角色死亡，则不抛出
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

void AProjectJBattleManager::OnAttackHit(int InCharacterID)
{
	check(InCharacterID == BattleContext.OrderArray[BattleContext.CurrentOrderIndex]);
	// 攻击命中，造成伤害
	auto Attacker = GetCurrentCharacter();
	auto WeaponConfig = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<
		FProjectJWeaponConfig>(Attacker->TempWeaponRowName, TEXT("OnAttackHit"));
	auto AttackASC = Attacker->GetAbilitySystemComponent();
	auto AttackerAttack = AttackASC->GetNumericAttribute(UProjectJCharacterAttributeSet::GetAttackAttribute());
	for (const auto& TargetID : BattleContext.AttackTargets)
	{
		auto Defender = BattleCharacterMap[TargetID];
		switch (WeaponConfig->AttackAbility.AttackCapability)
		{
			case EProjectJAttackCapability::Damage:
				{
					// Todo: 伤害计算规则： 护甲值每次战斗后都会恢复， HP则必须用药物恢复； 受到伤害，先直接扣除护甲值， 然后还遗留剩余的伤害，则添加GE_Damage， 更新Damage； UI上显示的HP, 始终为Health - Damage
					// 伤害计算流程
					auto DamageGEHandle = UProjectJGameBPFL::SimpleMakeGESpecHandle(Attacker.Get(), DamageEffect);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
						DamageGEHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Damage, AttackerAttack);
					AttackASC->ApplyGameplayEffectSpecToTarget(*DamageGEHandle.Data.Get(),
					                                           Defender->GetAbilitySystemComponent());
				}
				break;
			case EProjectJAttackCapability::Heal:
				{
					auto HealGEHandle = UProjectJGameBPFL::SimpleMakeGESpecHandle(Attacker.Get(), HealEffect);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
						HealGEHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Damage, -AttackerAttack);
					AttackASC->ApplyGameplayEffectSpecToTarget(*HealGEHandle.Data.Get(),
					                                           Defender->GetAbilitySystemComponent());
				}
				break;
		}
	}

	// 表演层，播放命中动画
	// 目标也需要播放该动画
	if  (WeaponConfig->AttackAbility.AttackCapability == EProjectJAttackCapability::Damage)
	{
		auto HitMontage = GetDefault<UProjectJGeneralSettings>()->HitMontage.LoadSynchronous();
		for (const auto& TargetID : BattleContext.AttackTargets)
		{
			auto Target = BattleCharacterMap[TargetID];
			if (Target->IsDead())
			{
				Target->ChangeAnimState(EProjectJCardAnimState::Death, FProjectJCharacterAniData::Empty);
				// 设置数据， 逻辑位置移动到-1
				Target->GetAbilitySystemComponent()->SetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute(), -1);
			}
			else
			{
				Target->Mesh->GetAnimInstance()->Montage_Play(HitMontage);
			}
		}
	}
	// Todo: Heal 效果特效
}

void AProjectJBattleManager::AfterAttackHit(int InCharacterID)
{
	check(InCharacterID == BattleContext.OrderArray[BattleContext.CurrentOrderIndex]);
	check(WaitSignals.Contains(SignalSimplePending));
	WaitSignals.Remove(SignalSimplePending);
}

void AProjectJBattleManager::OnIdleReturnToPosition(int InCharacterID)
{
	check(InCharacterID == BattleContext.OrderArray[BattleContext.CurrentOrderIndex]);
	check(WaitSignals.Contains(SignalSimplePending));
	// 攻击结束后， 此时卡牌已经回到原位， 检查是否需要调整站位
	TeamFillData.TeamMovingTo.Empty();
	TeamFillData.TeamMovingFrom.Empty();
	BuildTeamResetCharacters(1, BattleContext.TopTeam, TeamFillData.TeamMovingTo);
	BuildTeamResetCharacters(0, BattleContext.BottomTeam, TeamFillData.TeamMovingTo);
	
	if (TeamFillData.TeamMovingTo.Num() > 0)
	{
		TeamFillData.StartTime = GetWorld()->GetTimeSeconds();
		check(!TeamFillData.MovingTimerHandle.IsValid());

		// 构造移动起点数据
		for (const auto& Tuple : TeamFillData.TeamMovingTo)
		{
			auto Character = BattleCharacterMap[Tuple.Key];
			TeamFillData.TeamMovingFrom.Add(Character->ID, Character->GetActorLocation());
		}
		GetWorld()->GetTimerManager().SetTimer(TeamFillData.MovingTimerHandle, this, &AProjectJBattleManager::UpdateTeamFill, 0.01f, true);
	}
	else
	{
		WaitSignals.Remove(SignalSimplePending);
	}
}

void AProjectJBattleManager::BuildTeamResetCharacters(int32 TeamID, const TArray<int32>& InTeam, TMap<int32, FVector>& WriteMoving)
{
	TArray<TWeakObjectPtr<AProjectJCharacter>> AliveCharacters;
	for (const auto& CharacterID : InTeam)
	{
		if (BattleCharacterMap[CharacterID]->IsDead())
		{
			continue;
		}
		AliveCharacters.Add(BattleCharacterMap[CharacterID]);
	}

	// Sort by Position
	AliveCharacters.Sort([](const TWeakObjectPtr<AProjectJCharacter>& A, const TWeakObjectPtr<AProjectJCharacter>& B) {
		return A->GetAbilitySystemComponent()->GetNumericAttribute(UProjectJCharacterAttributeSet::GetPositionAttribute()) <
			B->GetAbilitySystemComponent()->GetNumericAttribute(UProjectJCharacterAttributeSet::GetPositionAttribute());
	});
	auto AliveCount = AliveCharacters.Num();
	// 此时站位应该为0，1...AliveCharacter-1
	for (int32 i = 0; i < AliveCharacters.Num(); i++)
	{
		auto Character = AliveCharacters[i];
		Character->GetAbilitySystemComponent()->SetNumericAttributeBase(UProjectJCharacterAttributeSet::GetPositionAttribute(), i);
			
		auto DesiredLocation = GetTeamPosition(TeamID, i, AliveCount);
		auto CurrentLocation = Character->GetActorLocation();

		if (FVector::Dist(DesiredLocation, CurrentLocation) > 5.f)
		{
			WriteMoving.Add(Character->ID, DesiredLocation);
		}
	}
}

void AProjectJBattleManager::UpdateTeamFill()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float Alpha = (CurrentTime - TeamFillData.StartTime) / TeamFillData.Duration;

	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(TeamFillData.MovingTimerHandle);

		for (const auto& Tuple : TeamFillData.TeamMovingTo)
		{
			auto Character = BattleCharacterMap[Tuple.Key];
			Character->SetActorLocation(Tuple.Value);
		}

		WaitSignals.Remove(SignalSimplePending);
	}
	else
	{
		// 
		float EasedAlpha = FMath::InterpExpoInOut(0.0f, 1.0f, Alpha);

		for (const auto& Tuple : TeamFillData.TeamMovingTo)
		{
			auto Character = BattleCharacterMap[Tuple.Key];
			auto StartPosition = TeamFillData.TeamMovingFrom[Tuple.Key];
			auto NewPosition = FMath::Lerp(StartPosition, Tuple.Value, EasedAlpha);
			Character->SetActorLocation(NewPosition);
		}
	}
}

