// Copyright 2016 GAMES ACADEMY GmbH

using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalTarget : TargetRules
{
	public SurvivalTarget(TargetInfo Target)
	{
		Type = TargetType.Game;

		bUsesSteam = true;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "Survival" } );
    }
}
