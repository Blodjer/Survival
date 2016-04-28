// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerState.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
	TeamNumber = 0;
}

void ASurvivalPlayerState::AssignToTeam(int32 Number)
{
	TeamNumber = Number;
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalPlayerState, TeamNumber);
}

