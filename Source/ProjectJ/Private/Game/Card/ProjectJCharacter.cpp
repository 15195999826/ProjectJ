// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "ProjectJ/ProjectJDWGlobal.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "Types/ProjectJCharacterConfig.h"
#include "UI/SpecialUI/ProjectJCharacterFloatPanel.h"

// Sets default values
AProjectJCharacter::AProjectJCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	// 手动设置ReceiveHardwareInput为true
	FloatWidgetComponent->SetupAttachment(Mesh, "Cube");


	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	AttributeSet = CreateDefaultSubobject<UProjectJCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AProjectJCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 初始ASC
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
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
		case EProjectJCardAnimState::DoAttack:
			{
				
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
		auto InitAttributeGESpecHandle = UProjectJGameBPFL::SimpleMakeGESpecHandle(this, GSettings->InitAttributeEffect);
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

void AProjectJCharacter::ChangeAnimState_Implementation(EProjectJCardAnimState InState,
	const FProjectJCharacterAniData& InData)
{
	AnimState = InState;
	AniData = InData;
	// 蓝图里，死亡等动画状态通过动画蓝图切换，部分则使用程序动画

	// Todo: 根据AniData， 和State， 设置实际的动画数据
	switch (AnimState) {
		case EProjectJCardAnimState::Idle:
			break;
		case EProjectJCardAnimState::AttackStart:
			{
				// Todo: 目前都用单体攻击
				// 动画表现为： 卡牌快速向后向上移动，并且向下旋转10°
				AttackAniData = FProgramAttackData();
				// 计算跟Target 0的角度差
				auto TargetLocation = AniData.TargetCards[0]->GetActorLocation();
				auto TargetDirection = TargetLocation - GetActorLocation();
				auto TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
				
				AttackAniData.TargetLocation = GetActorLocation() + FVector(-100, 0, 100);
				AttackAniData.TargetRotation = FRotator(-10, TargetRotation.Yaw, TargetRotation.Roll);
				AttackAniData.StartLocation = GetActorLocation();
				AttackAniData.StartRotation = GetActorRotation();
				AttackAniData.StartTime = GetWorld()->GetTimeSeconds();
				AttackAniData.Duration = 0.5f;

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
				AttackAniData = FProgramAttackData();
				AttackAniData.TargetLocation = AniData.TargetCards[0]->GetActorLocation();
				auto CurrentRotation = GetActorRotation();
				AttackAniData.TargetRotation = FRotator(0, CurrentRotation.Yaw, CurrentRotation.Roll);
				AttackAniData.StartLocation = GetActorLocation();
				AttackAniData.StartRotation = CurrentRotation;
				AttackAniData.StartTime = GetWorld()->GetTimeSeconds();
				AttackAniData.Duration = 0.5f;
				GetWorld()->GetTimerManager().SetTimer(
					DoAttackTimerHandle,
					this,
					&AProjectJCharacter::UpdateDoAttackAnimation,
					0.01f,
					true
				);
			}
			break;
		case EProjectJCardAnimState::Death:
			break;
		case EProjectJCardAnimState::WaitingAttack:
			{
				WaitingAttackTimer = 0;
				auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
				EventSystem->OnWaitingAttack.Broadcast(ID);
			}
			break;
		case EProjectJCardAnimState::OnAttackHit:
			{
				// 发送命中事件
				// 然后蓝图播放命中时的卡牌撞击动画
			}
			break;
	}
}

bool AProjectJCharacter::IsDead()
{
	auto Health = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetHealthAttribute());
	auto Damage = AbilitySystemComponent->GetNumericAttribute(UProjectJCharacterAttributeSet::GetDamageAttribute());
	return Health <= Damage;
}

void AProjectJCharacter::UpdateStartAttackAnimation()
{
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - AttackAniData.StartTime) / AttackAniData.Duration;
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
		
		auto NewLocation = FMath::Lerp(AttackAniData.StartLocation, AttackAniData.TargetLocation, EasedAlpha);
		auto NewRotation = FMath::Lerp(AttackAniData.StartRotation, AttackAniData.TargetRotation, EasedAlpha);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
	}
}

void AProjectJCharacter::UpdateDoAttackAnimation()
{
	// 撞击到目标位置
	auto CurrentTime = GetWorld()->GetTimeSeconds();
	auto Alpha = (CurrentTime - AttackAniData.StartTime) / AttackAniData.Duration;
	if (Alpha >= 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(DoAttackTimerHandle);
		// Animation ends
		ChangeAnimState(EProjectJCardAnimState::OnAttackHit, FProjectJCharacterAniData::Empty);
	}
	else
	{
		
		float EasedAlpha;
		FVector NewLocation;
		FRotator NewRotation;

		if (Alpha < 0.5f)
		{
			// First half of the animation: move backward
			EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha * 2.0f, 2.0f);
			NewLocation = FMath::Lerp(AttackAniData.StartLocation, AttackAniData.StartLocation + FVector(-50, 0, 0), EasedAlpha);
			NewRotation = FMath::Lerp(AttackAniData.StartRotation, AttackAniData.TargetRotation, EasedAlpha);
		}
		else
		{
			// Second half of the animation: move forward
			EasedAlpha = FMath::InterpExpoIn(0.0f, 1.0f, (Alpha - 0.5f) * 2);
			NewLocation = FMath::Lerp(AttackAniData.StartLocation + FVector(-50, 0, 0), AttackAniData.TargetLocation, EasedAlpha);
			NewRotation = FMath::Lerp(AttackAniData.TargetRotation, AttackAniData.TargetRotation, EasedAlpha);
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

