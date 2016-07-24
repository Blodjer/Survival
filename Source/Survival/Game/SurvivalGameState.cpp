// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameState.h"
#include "Survival/Level/SurvivalLevelScriptActor.h"


ASurvivalGameState::ASurvivalGameState()
{
	MatchTimeStartOffset = 0.0f;
}

void ASurvivalGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (HasAuthority())
	{
		UpdateServerTimeSeconds();
		MatchTimeStartOffset = GetServerWorldTimeSeconds();
	}

	if (GetLevel()->LevelScriptActor)
	{
		ASurvivalLevelScriptActor* SurvivalLevelScriptActor = Cast<ASurvivalLevelScriptActor>(GetLevel()->LevelScriptActor);
		if (SurvivalLevelScriptActor)
		{
			SurvivalLevelScriptActor->HandleMatchHasStarted();
		}
	}
}

void ASurvivalGameState::SetMatchProperties(float LengthOfDay, float StartTimeOfDay, TArray<FTeamInfo> Teams)
{
	GameMode_LengthOfDay = LengthOfDay;
	GameMode_StartTimeOfDay = StartTimeOfDay;
	GameMode_Teams = Teams;
	Seed = FMath::Rand();
}

float ASurvivalGameState::GetMatchTime()
{
	return HasMatchStarted() ? GetServerWorldTimeSeconds() - MatchTimeStartOffset : 0.0f;
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

int32 ASurvivalGameState::GetReplicatedSeed() const
{
	return Seed;
}

void ASurvivalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalGameState, MatchTimeStartOffset);

	DOREPLIFETIME_CONDITION(ASurvivalGameState, GameMode_LengthOfDay, COND_InitialOnly);

#if !UE_BUILD_SHIPPING
	DOREPLIFETIME(ASurvivalGameState, GameMode_StartTimeOfDay);
#else
	DOREPLIFETIME_CONDITION(ASurvivalGameState, GameMode_StartTimeOfDay, COND_InitialOnly);
#endif

	DOREPLIFETIME_CONDITION(ASurvivalGameState, GameMode_Teams, COND_InitialOnly);

	DOREPLIFETIME_CONDITION(ASurvivalGameState, Seed, COND_InitialOnly)
}
