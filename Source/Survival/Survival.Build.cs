// Copyright 2016 GAMES ACADEMY GmbH

using UnrealBuildTool;

public class Survival : ModuleRules
{
	public Survival(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "MovieScene" });

        PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent", "LevelSequence" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Setup online subsystem
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils" });
	}
}
