// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameMode.h"
#include "SurvivalGameState.h"


ASurvivalGameMode::ASurvivalGameMode()
{
	LengthOfDay = 60.0f;
	StartTimeOfDay = 8.5f;
}

void ASurvivalGameMode::InitGameState()
{
	Super::InitGameState();

	ASurvivalGameState* SurvivalGameState = GetGameState<ASurvivalGameState>();
	if (SurvivalGameState)
	{
		SurvivalGameState->SetMatchProperties(LengthOfDay, StartTimeOfDay);
	}
}
