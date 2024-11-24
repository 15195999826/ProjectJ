using UnrealBuildTool;

public class LomoCharacterCreatorEditor : ModuleRules
{
    public LomoCharacterCreatorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core","LomoCharacterCreator"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                
                "UnrealEd",
                "PropertyEditor",
                "EditorStyle",
                "BlueprintGraph",
                "GraphEditor",
                "Kismet",
                "KismetCompiler",
                "Projects",
                "AssetTools",
            }
        );
    }
}