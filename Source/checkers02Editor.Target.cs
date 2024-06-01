// Copyright Stephen Montague.

using UnrealBuildTool;
using System.Collections.Generic;

public class checkers02EditorTarget : TargetRules
{
	public checkers02EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "checkers02" } );
	}
}
