// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"
#include "Helper/SurvivalCheatManager.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
	CheatClass = USurvivalCheatManager::StaticClass();
}

void ASurvivalPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}
