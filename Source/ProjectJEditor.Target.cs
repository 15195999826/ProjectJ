// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectJEditorTarget : TargetRules
{
	public ProjectJEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("ProjectJ");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider() {
		ExtraModuleNames.AddRange(new string[] { "ProjectJEditor" });
	}
}
