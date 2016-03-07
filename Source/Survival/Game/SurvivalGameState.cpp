// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameState.h"


ASurvivalGameState::ASurvivalGameState()
{
	MatchTimeStartOffset = 0.0f;
}

void ASurvivalGameState::SetMatchProperties(float LengthOfDay, float StartTimeOfDay)
{
	GameMode_LengthOfDay = LengthOfDay;
	GameMode_StartTimeOfDay = StartTimeOfDay;
}

float ASurvivalGameState::GetMatchTime()
{
	return GetServerWorldTimeSeconds() - MatchTimeStartOffset;
}

float ASurvivalGameState::GetTimeOfDay()
{
	return FMath::Fmod((GetMatchTime() + GameMode_StartTimeOfDay / (24.0f / GameMode_LengthOfDay)) * (24.0f / GameMode_LengthOfDay), 24.0f);
}

float ASurvivalGameState::GetLengthOfDay()
{
	return GameMode_LengthOfDay;
}

void ASurvivalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalGameState, MatchTimeStartOffset);

	DOREPLIFETIME(ASurvivalGameState, GameMode_LengthOfDay);
	DOREPLIFETIME(ASurvivalGameState, GameMode_StartTimeOfDay);
}
