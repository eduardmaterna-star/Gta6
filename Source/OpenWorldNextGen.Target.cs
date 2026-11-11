using UnrealBuildTool;
using System.Collections.Generic;

public class OpenWorldNextGenTarget : TargetRules
{
    public OpenWorldNextGenTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("OpenWorldNextGen");
    }
}