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

void ASurvivalGameMode::Killed(const UDamageType* DamageType, AController* Killer, AController* KilledPlayer)
{
	if (Killer != nullptr && Killer->PlayerState != nullptr && KilledPlayer != nullptr && KilledPlayer->PlayerState != nullptr)
	{
		FString KillerName = Killer->PlayerState->GetHumanReadableName();
		FString KilledPlayerName = KilledPlayer->PlayerState->GetHumanReadableName();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString(KillerName + " killed " + KilledPlayerName));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DamageType->bCausedByWorld ? "World killed you" : "unknown killer");
	}
}
