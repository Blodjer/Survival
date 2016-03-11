// Copyright 2016 GAMES ACADEMY GmbH

using UnrealBuildTool;

public class Survival : ModuleRules
{
	public Survival(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Setup online subsystem
		PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils" });
		if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		{
			if (UEBuildConfiguration.bCompileSteamOSS == true)
			{
				DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
			}
			else
			{
				DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
			}
		}
	}
}
