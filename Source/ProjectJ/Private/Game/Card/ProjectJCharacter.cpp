// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJEffectActor.h"
#include "Game/ProjectJGameBFL.h"
#include "Game/ProjectJLevelSettingActor.h"
#include "Game/GAS/ProjectJAbilitySystemComponent.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "Interface/ProjectJAttackEffectInterface.h"
#include "ProjectJ/ProjectJDWGlobal.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Types/ProjectJCharacterConfig.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "UI/SpecialUI/ProjectJCharacterFloatPanel.h"


// Sets default values
AProjectJCharacter::AProjectJCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	// 手动设置ReceiveHardwareInput为true
	FloatWidgetComponent->SetupAttachment(Mesh, "Cube");


	AbilitySystemComponent = CreateDefaultSubobject<UProjectJAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	AttributeSet = CreateDefaultSubobject<UProjectJCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AProjectJCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 初始ASC
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	BackgroundDynamicMaterial = Mesh->CreateDynamicMaterialInstance(2);
}

// Called every frame
void AProjectJCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (AnimState) {
		case EProjectJCardAnimState::WaitingAttack:
			{
				// 随机抖动
				// WaitingAttackTimer += DeltaTime;
				// if (WaitingAttackTimer >= 0.033f)
				// {
				// 	WaitingAttackTimer = 0;
				// 	FVector NewLocation = AniData.LocationPayload;
				// 	NewLocation.X += FMath::FRandRange(-5.f, 5.f);
				// 	NewLocation.Y += FMath::FRandRange(-5.f, 5.f);
				// 	NewLocation.Z += FMath::FRandRange(-5.f, 5.f);
				// 	SetActorLocation(NewLocation);
				// }
			}
			break;
		case EProjectJCardAnimState::AfterDoAttack:
			{
				// 如果卡牌Z轴值大于1， 怎快速下落到1
				if (GetActorLocation().Z > 1 || GetActorRotation().Pitch > 0.1f)
				{
					auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
					auto NewLocation = GetActorLocation();
					NewLocation.Z -= LevelSettings->ProgramAnimationSettings.OnAttackHitDropDownSpeed * DeltaTime;
					auto NewRotation = GetActorRotation();
					NewRotation.Pitch -= LevelSettings->ProgramAnimationSettings.OnAttackHitDropDownRotateSpeed * DeltaTime;
					if (NewLocation.Z <= 1)
					{
						NewLocation.Z = 1;
					}
					if (NewRotation.Pitch <= 0)
					{
						NewRotation.Pitch = 0;
					}
					SetActorLocation(NewLocation);
					SetActorRotation(NewRotation);
				}
			}
			break;
		default:
			break;
	}
}

void AProjectJCharacter::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	Title->SetText(FText::FromString(InRowName.ToString()));
	auto Config = GetDefault<UProjectJDataTableSettings>()->CharacterTable.LoadSynchronous()->FindRow<
		FProjectJCharacterConfig>(InRowName, TEXT(""));

	if (Config)
	{
		// 给予属性
		auto GSettings = GetDefault<UProjectJGeneralSettings>();
		auto InitAttributeGESpecHandle = UProjectJGameBFL::SimpleMakeGESpecHandle(this, GSettings->InitAttributeEffect);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Health, Config->Health);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Attack, Config->Attack);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(InitAttributeGESpecHandle, ProjectJGameplayTags::SetByCaller_Attribute_Battle_Speed, Config->Speed);

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitAttributeGESpecHandle.Data.Get());

		auto FloatPanel = Cast<UProjectJCharacterFloatPanel>(FloatWidgetComponent->GetWidget());
		FloatPanel->BindCharacter(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterConfig %s not found"), *InRowName.ToString());
	}
}

FName AProjectJCharacter::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}

void AProjectJCharacter::PostAfterAttackHit()
{
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->AfterAttackHit.Broadcast(ID);
}

bool AProjectJCharacter::GetIsDead_Implementation()
{
	return IsDead();
}

bool AProjectJCharacter::GetIsAtTopTeam_Implementation()
{
	return IsAtTopTeam();
}

bool AProjectJCharacter::IsDead()
{
	auto Health = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetHealthAttribute());
	auto Damage = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetDamageAttribute());
	return Health <= Damage;
}

bool AProjectJCharacter::IsAtTopTeam()
{
	return static_cast<int32>(AbilitySystemComponent->GetNumericAttribute(
		UProjectJCharacterAttributeSet::GetTeamAttribute())) == 1;
}

void AProjectJCharacter::ChangeAnimState_Implementation(EProjectJCardAnimState InState,
                                                        const FProjectJCharacterAniData& InData)
{
	AnimState = InState;
	AniData = InData;
	// 蓝图里，死亡等动画状态通过动画蓝图切换，部分则使用程序动画

	// Todo: 根据AniData， 和State， 设置实际的动画数据
	switch (AnimState) {
		case EProjectJCardAnimState::Idle:
			{
				if (!InData.IsEmpty)
				{
					// 有动画数据，设置位置， 返回至数据中的位置
					GeneralAniData = FProgramAnimationData();
					GeneralAniData.TargetLocation = InData.LocationPayload;
					GeneralAniData.TargetRotation = FRotator::ZeroRotator;
					GeneralAniData.StartLocation = GetActorLocation();
					GeneralAniData.StartRotation = GetActorRotation();
					GeneralAniData.StartTime = GetWorld()->GetTimeSeconds();
					GeneralAniData.Duration = 0.5f;

					GetWorld()->GetTimerManager().SetTimer(
						IdleReturnToPositionTimerHandle,
						this,
						&AProjectJCharacter::UpdateIdleReturnToPositionAnimation,
						0.01f,
						true
					);
				}
			}
			break;
		case EProjectJCardAnimState::AttackStart:
			{
				auto IsInTop = IsAtTopTeam();

				// Todo: 以后实现， 武器AttackStart特效动画等配置
				// auto WeaponConfig = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<
				// 	FProjectJWeaponConfig>(TempWeaponRowName, TEXT("ChangeAnimState"));
				
				auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
				// Todo: 目前都用单体攻击
				// 动画表现为： 卡牌快速向后向上移动，并且向下旋转10°
				GeneralAniData = FProgramAnimationData();
				// 计算跟Target 0的角度差
				auto TargetLocation = AniData.LocationPayload;
				auto TargetDirection = TargetLocation - GetActorLocation();
				
				auto TargetRotation = IsInTop ?
					FRotationMatrix::MakeFromX(-TargetDirection).Rotator():
					FRotationMatrix::MakeFromX(TargetDirection).Rotator();

				float Reverse = IsInTop ? -1 : 1;
				
				GeneralAniData.TargetLocation = GetActorLocation() + LevelSettings->ProgramAnimationSettings.AttackStartTargetLocationOffset * FVector(Reverse, 1, 1);
				GeneralAniData.TargetRotation = FRotator(Reverse * LevelSettings->ProgramAnimationSettings.AttackStartTargetPitch, TargetRotation.Yaw, TargetRotation.Roll);
				GeneralAniData.StartLocation = GetActorLocation();
				GeneralAniData.StartRotation = GetActorRotation();
				GeneralAniData.StartTime = GetWorld()->GetTimeSeconds();
				GeneralAniData.Duration = LevelSettings->ProgramAnimationSettings.AttackStartDuration;

				GetWorld()->GetTimerManager().SetTimer(
					StartAttackTimerHandle,
					this,
					&AProjectJCharacter::UpdateStartAttackAnimation,
					0.01f,
					true
				);
			}
			break;
		case EProjectJCardAnimState::DoAttack:
			{
				auto WeaponConfig = GetDefault<UProjectJDataTableSettings>()->WeaponTable.LoadSynchronous()->FindRow<
					FProjectJWeaponConfig>(TempWeaponRowName, TEXT("ChangeAnimState"));

				float MaxDuration = 0;
				float MaxBackSwing = -1.f;
				if (WeaponConfig->AttackAbility.AnimationConfigs.Num() > 0)
				{
					for (const auto& AnimConfig: WeaponConfig->AttackAbility.AnimationConfigs)
					{
						if (AnimConfig.PerformStage == EProjectJBattleStage::CharacterDoAttack)
						{
							switch (AnimConfig.AnimationType) {
								case EProjectJAbilityAnimationType::Program:
									{
										float BackSwing;
										if (AnimConfig.CustomKV.Contains(ProjectJGlobal::Program_Ani_NameKey))
										{
											BackSwing = ProgramDoAttack(FName(*AnimConfig.CustomKV[ProjectJGlobal::Program_Ani_NameKey]));
										}
										else
										{
											BackSwing = ProgramDoAttack(ProjectJGlobal::Program_Ani_Knock);
										}

										auto ProtectDuration = GeneralAniData.Duration + 0.067f;
										if (ProtectDuration > MaxDuration)
										{
											MaxDuration = ProtectDuration;
										}

										if (BackSwing > MaxBackSwing)
										{
											MaxBackSwing = BackSwing;
										}
									}
								break;
								case EProjectJAbilityAnimationType::Effect:
									{
										// Todo: 暂时只实现， 在位置播放特效， 至于特效位置本身，通过调整特效Actor中对应组件的位置来实现
										auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();

										AProjectJEffectActor* GetConfigEffectActor = nullptr;
										for (const auto& TargetCharacter: AniData.TargetCards)
										{
											AProjectJEffectActor* EffectActor = ContextSystem->GetEffectActor(
												AnimConfig.Resource);
											EffectActor->SetActorLocation(TargetCharacter->GetActorLocation());
											EffectActor->StartEffect();

											if (GetConfigEffectActor == nullptr)
											{
												GetConfigEffectActor = EffectActor;
											}
										}
										
										auto EffectDuration = GetConfigEffectActor->GetDuration();
										if (EffectDuration > MaxDuration)
										{
											MaxDuration = EffectDuration;
										}

										if (AnimConfig.GiveAttackPoint)
										{
											
											auto AttackPoint = IProjectJAttackEffectInterface::Execute_GetAttackPoint(GetConfigEffectActor);
											if (EffectDuration <= AttackPoint)
											{
												UE_LOG(LogTemp, Error, TEXT("配置错误, Effect Duration: %f  less than AttackPoint:%f, Name: %s"), EffectDuration, AttackPoint, *AnimConfig.Resource->GetFName().ToString());
												AttackPoint = EffectDuration;
											}
											
											FTimerHandle UnUsedDelegate;
											GetWorld()->GetTimerManager().SetTimer(
												UnUsedDelegate,
												[this]()
												{
													auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
													EventSystem->OnAttackHit.Broadcast(ID);
												},
												AttackPoint,
												false
											);
										}
									}
									break;
							}
						}
					}
				}
				else
				{
					MaxBackSwing = ProgramDoAttack(ProjectJGlobal::Program_Ani_Knock);
					MaxDuration = GeneralAniData.Duration + 0.067f;
				}

				// 播放动画或者特效， 或者都会播放， 然后在持续时间结束后，进入后摇阶段，目前后摇阶段用于将卡牌位置降落到地面与Pitch归零
				// Animation ends
				FTimerHandle UnUsedAfterAttackDelegate;
				GetWorld()->GetTimerManager().SetTimer(
					UnUsedAfterAttackDelegate,
					[this, MaxBackSwing]()
					{
						check(!ProgramDoAttackTimerHandle.IsValid());
						FProjectJCharacterAniData AniData;
						AniData.FloatPayload = MaxBackSwing;
						ChangeAnimState(EProjectJCardAnimState::AfterDoAttack, AniData);
					},
					MaxDuration,
					false
				);
			}
			break;
		case EProjectJCardAnimState::Death:
			{
				// Todo: 未来制作亡语技能时， 先播放亡语技能动画，再播放溶解特效
				auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
				GeneralAniData = FProgramAnimationData();
				GeneralAniData.Duration =  LevelSettings->ProgramAnimationSettings.DissolveDuration;
				auto StartDelay = LevelSettings->ProgramAnimationSettings.StartDissolveDelay;
				FTimerHandle UnUsedDelegate;
				GetWorld()->GetTimerManager().SetTimer(
					UnUsedDelegate,
					[this]()
					{
						GeneralAniData.StartTime = GetWorld()->GetTimeSeconds();
						GetWorld()->GetTimerManager().SetTimer(
							DissolveTimerHandle,
							this,
							&AProjectJCharacter::UpdateDissolveAnimation,
							0.01f,
							true
						);
					},
					StartDelay,
					false
				);
			}
			break;
		case EProjectJCardAnimState::WaitingAttack:
			{
				WaitingAttackTimer = 0;
				auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
				EventSystem->OnWaitingAttack.Broadcast(ID);
			}
			break;
		case EProjectJCardAnimState::AfterDoAttack:
			{
				if (AniData.FloatPayload > 0)
				{
					FTimerHandle UnUsedDelegate;
					GetWorld()->GetTimerManager().SetTimer(
						UnUsedDelegate,
						this,
						&AProjectJCharacter::PostAfterAttackHit,
						AniData.FloatPayload,
						false
					);
				}
				else
				{
					PostAfterAttackHit();
				}
			}
			break;
	}
}

void AProjectJCharacter::UpdateStartAttackAnimation()
{
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - GeneralAniData.StartTime) / GeneralAniData.Duration;
	if (Alpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartAttackTimerHandle);
		// 动画结束
		FProjectJCharacterAniData WaitingAttackAniData;
		WaitingAttackAniData.LocationPayload = GetActorLocation();
		ChangeAnimState(EProjectJCardAnimState::WaitingAttack, WaitingAttackAniData);
	}
	else
	{
		auto EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
		
		auto NewLocation = FMath::Lerp(GeneralAniData.StartLocation, GeneralAniData.TargetLocation, EasedAlpha);
		auto NewRotation = FMath::Lerp(GeneralAniData.StartRotation, GeneralAniData.TargetRotation, EasedAlpha);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
	}
}

float AProjectJCharacter::ProgramDoAttack(const FName& InName)
{
	if (InName == ProjectJGlobal::Program_Ani_Knock)
	{
		auto IsInTop = IsAtTopTeam();
		float Reverse = IsInTop ? -1 : 1;
		auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
		GeneralAniData = FProgramAnimationData();
		GeneralAniData.TargetLocation = AniData.TargetCards[0]->GetActorLocation() + Reverse * LevelSettings
			->ProgramAnimationSettings.DoAttackTargetLocationOffset;
		auto CurrentRotation = GetActorRotation();
		GeneralAniData.TargetRotation = FRotator(0, CurrentRotation.Yaw, CurrentRotation.Roll);
		GeneralAniData.StartLocation = GetActorLocation();
		GeneralAniData.StartRotation = CurrentRotation;
		GeneralAniData.StartTime = GetWorld()->GetTimeSeconds();
		GeneralAniData.Duration = LevelSettings->ProgramAnimationSettings.DoAttackDuration;
		AlreadyPostAttackHit = false;
		GetWorld()->GetTimerManager().SetTimer(
			ProgramDoAttackTimerHandle,
			this,
			&AProjectJCharacter::UpdateKnockAnimation,
			0.01f,
			true
		);

		return LevelSettings->ProgramAnimationSettings.KnockBackSwing;
	}

	return -1.f;
}

void AProjectJCharacter::UpdateKnockAnimation()
{
	// 撞击到目标位置
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - GeneralAniData.StartTime) / GeneralAniData.Duration;
	if (Alpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(ProgramDoAttackTimerHandle);
	}
	else
	{
		
		float EasedAlpha;
		FVector NewLocation;
		FRotator NewRotation;

		auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
		float Reverse = IsAtTopTeam() ? -1 : 1;
		if (Alpha < 0.5f)
		{
			
			// First half of the animation: move backward
			EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha * 2.0f, 2.0f);
			NewLocation = FMath::Lerp(GeneralAniData.StartLocation,
			                          GeneralAniData.StartLocation + FVector(
				                          Reverse * LevelSettings->ProgramAnimationSettings.DoAttackMoveBackWorldOffset,
				                          0, 0), EasedAlpha);
			NewRotation = FMath::Lerp(GeneralAniData.StartRotation,
			                          GeneralAniData.StartRotation + FRotator(
				                          Reverse * LevelSettings->ProgramAnimationSettings.DoAttackMoveBackWorldPitch,
				                          0, 0), EasedAlpha);
		}
		else
		{
			// Second half of the animation: move forward
			EasedAlpha = FMath::InterpExpoIn(0.0f, 1.0f, (Alpha - 0.5f) * 2);
			NewLocation = FMath::Lerp(GeneralAniData.StartLocation + FVector(
				                          Reverse * LevelSettings->ProgramAnimationSettings.DoAttackMoveBackWorldOffset,
				                          0, 0), GeneralAniData.TargetLocation, EasedAlpha);
			NewRotation = FMath::Lerp(GeneralAniData.StartRotation + FRotator(
				                          Reverse * LevelSettings->ProgramAnimationSettings.DoAttackMoveBackWorldPitch,
				                          0, 0), GeneralAniData.TargetRotation, EasedAlpha);
			
			if (!AlreadyPostAttackHit && EasedAlpha >= LevelSettings->ProgramAnimationSettings.DoAttackPlayMontagePercent)
			{
				// 发送命中事件
				auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
				EventSystem->OnAttackHit.Broadcast(ID);
				// 在BattleManager中已经计算了伤害，此时目标是否死亡已知
				
				// Play will hit montage
				AlreadyPostAttackHit = true;
				auto HitMontage = GetDefault<UProjectJGeneralSettings>()->HitMontage.LoadSynchronous();
				// PlayMontage
				Mesh->GetAnimInstance()->Montage_Play(HitMontage);
			}
		}

		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
		// auto EasedAlpha = FMath::InterpExpoIn(0.0f, 1.0f, Alpha);
		//
		// auto NewLocation = FMath::Lerp(AttackAniData.StartLocation, AttackAniData.TargetLocation, EasedAlpha);
		// auto NewRotation = FMath::Lerp(AttackAniData.StartRotation, AttackAniData.TargetRotation, EasedAlpha);
		// SetActorLocation(NewLocation);
		// SetActorRotation(NewRotation);
	}
}

void AProjectJCharacter::UpdateIdleReturnToPositionAnimation()
{
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - GeneralAniData.StartTime) / GeneralAniData.Duration;
	if (Alpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(IdleReturnToPositionTimerHandle);
		SetActorLocation(GeneralAniData.TargetLocation);
		SetActorRotation(GeneralAniData.TargetRotation);
		auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
		EventSystem->OnIdleReturnToPosition.Broadcast(ID);
	}
	else
	{
		auto EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
		auto NewLocation = FMath::Lerp(GeneralAniData.StartLocation, GeneralAniData.TargetLocation, EasedAlpha);
		auto NewRotation = FMath::Lerp(GeneralAniData.StartRotation, GeneralAniData.TargetRotation, EasedAlpha);
		
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
	}
	
}

void AProjectJCharacter::UpdateDissolveAnimation()
{
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - GeneralAniData.StartTime) / GeneralAniData.Duration;
	if (Alpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
		SetActorLocation(UProjectJContextSystem::HiddenLocation);
	}
	else
	{
		auto DissolveAlpha = FMath::Lerp(0.0f, 1.0f, Alpha);
		BackgroundDynamicMaterial->SetScalarParameterValue("Apperance", DissolveAlpha);
	}
}

