// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalCheatManager.h"
#include "Player/SurvivalPlayerController.h"
#include "Game/SurvivalGameState.h"

void USurvivalCheatManager::SetTimeOfDay(float TimeOfDay)
{
	ASurvivalPlayerController* PlayerController = GetOuterASurvivalPlayerController();
	if (PlayerController->GetWorld())
	{
		ASurvivalGameState* SurvivalGameState = Cast<ASurvivalGameState>(PlayerController->GetWorld()->GetGameState());
		if (SurvivalGameState)
		{
			SurvivalGameState->SetTimeOfDay(TimeOfDay);
		}
	}
}
