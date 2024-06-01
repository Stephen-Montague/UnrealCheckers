// Copyright Stephen Montague.

using UnrealBuildTool;
using System.Collections.Generic;

public class checkers02Target : TargetRules
{
	public checkers02Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "checkers02" } );
	}
}
