// Copyright 2016 GAMES ACADEMY GmbH

using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalTarget : TargetRules
{
	public SurvivalTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		bUsesSteam = true;

        ExtraModuleNames.Add("Survival");
    }
}
