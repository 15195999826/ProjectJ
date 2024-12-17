// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ProjectJPlayerControllerBase.h"

#include "EngineUtils.h"
#include "SpringArmCameraActor.h"
#include "Widgets/SWidget.h"
// 注意，这个Include不能删除
#include "ProjectJ/ProjectJLogChannels.h"
#include "Widgets/SViewport.h"

void AProjectJPlayerControllerBase::BeginPlay()
{
	for (TActorIterator<ASpringArmCameraActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASpringArmCameraActor* CameraActor = *ActorItr;
		if (CameraActor->Tags.Contains("MainCamera"))
		{
			CurrentCamera = CameraActor;
			CameraActor->BindInput(this);
			// 设置当前激活的相机
			this->SetViewTarget(CameraActor);
			break;
		}
	}
	Super::BeginPlay();
}

void AProjectJPlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TSharedPtr ViewPort = FSlateApplication::Get().GetGameViewport();

	bool OverWidget = false;
	if (ViewPort.IsValid())
	{
		FWidgetPath WidgetUnderMouse = FSlateApplication::Get().LocateWindowUnderMouse(FSlateApplication::Get().GetCursorPos() , FSlateApplication::Get().GetInteractiveTopLevelWindows(), true);
		
		OverWidget = !(WidgetUnderMouse.IsValid() &&  WidgetUnderMouse.GetLastWidget() == ViewPort.ToSharedRef());
		// UE_LOG(LogTheOne, Log, TEXT("OverWidget: %d, %s"), OverWidget, *WidgetUnderMouse.GetLastWidget()->GetWidgetClass().GetWidgetType().ToString());
	}
	SampleMouseState(LeftMouseState, WasInputKeyJustPressed(EKeys::LeftMouseButton), IsInputKeyDown(EKeys::LeftMouseButton));
	SampleMouseState(RightMouseState, WasInputKeyJustPressed(EKeys::RightMouseButton), IsInputKeyDown(EKeys::RightMouseButton));
	FHitResult HitResult;
	bool HitGround = false;
	AActor* CanHitActor = nullptr;
	FVector HitGroundLocation;
	// ATheOneCharacterBase* HitCharacter = nullptr;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit)
	{
		auto HitActor = HitResult.GetActor();
		if (HitActor)
		{
			if (HitResult.GetActor()->ActorHasTag("Ground"))
			{
				CurrentCamera->OnHitGround(HitResult.Location);
				HitGround = true;
			}
			else if (HitResult.GetActor()->ActorHasTag("CanHit"))
			{
				CurrentCamera->OnHitCard(HitActor);
				CanHitActor = HitActor;
			}

			UE_LOG(LogProjectJ, Log, TEXT("HitActor: %s"), *HitActor->GetName());
		}
	}
	
	CustomTick(DeltaSeconds, OverWidget, HitGround, HitGroundLocation, CanHitActor);
}

void AProjectJPlayerControllerBase::SampleMouseState(EProjectJMouseState& MouseState, bool IsJustPressed, bool IsDown)
{
	switch (MouseState)
	{
		case EProjectJMouseState::Idle:
			if (IsJustPressed)
			{
				UE_LOG(LogProjectJ, Log, TEXT("MouseState: Press"));
				MouseState = EProjectJMouseState::Press;
			}
			break;
		case EProjectJMouseState::Press:
			if (IsDown)
			{
				UE_LOG(LogProjectJ, Log, TEXT("MouseState: Pressing"));
				MouseState = EProjectJMouseState::Pressing;
			}
			else
			{
				UE_LOG(LogProjectJ, Log, TEXT("MouseState: Release"));
				MouseState = EProjectJMouseState::Release;
			}
			break;
		case EProjectJMouseState::Pressing:
			if (!IsDown)
			{
				UE_LOG(LogProjectJ, Log, TEXT("MouseState: Release"));
				MouseState = EProjectJMouseState::Release;
			}
			break;
		case EProjectJMouseState::Release:
			UE_LOG(LogProjectJ, Log, TEXT("MouseState: Idle"));
			MouseState = EProjectJMouseState::Idle;
			break;
		default: ;
	}
}
