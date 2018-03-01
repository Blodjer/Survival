// Copyright 2016 GAMES ACADEMY GmbH

using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalEditorTarget : TargetRules
{
	public SurvivalEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.Add("Survival");
	}
}
