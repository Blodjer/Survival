// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameState.h"


ASurvivalGameState::ASurvivalGameState()
{
	MatchTimeStartOffset = 0.0f;
}

void ASurvivalGameState::SetMatchProperties(float LengthOfDay, float StartTimeOfDay, TArray<FTeamInfo> Teams)
{
	GameMode_LengthOfDay = LengthOfDay;
	GameMode_StartTimeOfDay = StartTimeOfDay;
	GameMode_Teams = Teams;
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

TArray<FTeamInfo>& ASurvivalGameState::GetTeamInfos()
{
	return GameMode_Teams;
}

FTeamInfo& ASurvivalGameState::GetTeamInfo(int32 TeamIdx)
{
	return GameMode_Teams[TeamIdx];
}

int32 ASurvivalGameState::GetNumberOfTeams()
{
	return GameMode_Teams.Num();
}

void ASurvivalGameState::SetTimeOfDay(float TimeOfDay)
{
	if (TimeOfDay >= 0.0f)
	{
		GameMode_StartTimeOfDay = (FMath::Fmod(TimeOfDay, 24.0f) - GetTimeOfDay()) + GameMode_StartTimeOfDay;
	}
}

void ASurvivalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalGameState, MatchTimeStartOffset);

	DOREPLIFETIME(ASurvivalGameState, GameMode_LengthOfDay);
	DOREPLIFETIME(ASurvivalGameState, GameMode_StartTimeOfDay);

	DOREPLIFETIME(ASurvivalGameState, GameMode_Teams);
}
