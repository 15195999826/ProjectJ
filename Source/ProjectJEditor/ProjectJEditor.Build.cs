using UnrealBuildTool;

public class ProjectJEditor : ModuleRules
{
    public ProjectJEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                "ProjectJEditor"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EditorFramework",
                "UnrealEd",
                "PhysicsCore",
                "GameplayTagsEditor",
                "GameplayTasksEditor",
                "GameplayAbilities",
                "GameplayAbilitiesEditor",
                "UMG",
                "UMGEditor",
                "Blutility",
                "PropertyEditor",
                "PythonScriptPlugin",
                "ProjectJ"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "ToolMenus",
                "EditorStyle",
                "DataValidation",
                "MessageLog",
                "Projects",
                "DeveloperToolSettings",
                "CollectionManager",
                "SourceControl",
                "Chaos",
                "AIModule"
            }
        );
        
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
    }
}