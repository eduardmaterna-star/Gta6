using UnrealBuildTool;

public class OpenWorldNextGen : ModuleRules
{
    public OpenWorldNextGen(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem"
        });
    }
}