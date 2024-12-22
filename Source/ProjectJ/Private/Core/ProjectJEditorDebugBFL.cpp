// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ProjectJEditorDebugBFL.h"

#if WITH_EDITOR
#include "Editor.h"
#endif


void UProjectJEditorDebugBFL::DebugPauseEditor()
{
#if WITH_EDITOR
	GEditor->PlayWorld->bDebugPauseExecution = true;
#endif
}
