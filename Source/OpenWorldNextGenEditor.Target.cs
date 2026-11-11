using UnrealBuildTool;
using System.Collections.Generic;

public class OpenWorldNextGenEditorTarget : TargetRules
{
    public OpenWorldNextGenEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("OpenWorldNextGen");
    }
}