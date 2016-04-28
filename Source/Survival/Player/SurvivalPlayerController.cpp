// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{

}

void ASurvivalPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}
