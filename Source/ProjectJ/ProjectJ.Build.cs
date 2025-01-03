// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
// using System;
// using System.IO;

public class ProjectJ : ModuleRules
{
	public ProjectJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "../../Config/Config", "Unlua_LocText.ini"));

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "UMG", "Paper2D",
			"Slate", "SlateCore", "UnLua"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
			{ "GameplayTags", "GameplayTasks", "Niagara", "DeveloperSettings"});

		if (Target.bBuildEditor) {
			PrivateDependencyModuleNames.AddRange(new string[]{"UnrealEd"});
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
