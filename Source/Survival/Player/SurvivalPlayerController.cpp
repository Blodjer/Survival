// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"
#include "Helper/SurvivalCheatManager.h"
#include "Game/SurvivalGameMode.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
	CheatClass = USurvivalCheatManager::StaticClass();
}

void ASurvivalPlayerController::UnFreeze()
{
	Super::UnFreeze();

	ServerRestartPlayer();
}

float ASurvivalPlayerController::GetRemainingRespawnTime()
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_UnFreeze);
}

float ASurvivalPlayerController::GetMinDieDelay()
{
	AGameState const* const GameState = GetWorld()->GameState;
	return ((GameState != NULL) && (GameState->GameModeClass != NULL)) ? GetDefault<ASurvivalGameMode>(GameState->GameModeClass)->MinDieDelay : 0.0f;
}

void ASurvivalPlayerController::MatchStartCountdown_Implementation(float RemainingTime)
{
	OnMatchStartCountdown(RemainingTime);
}

void ASurvivalPlayerController::MatchHasEnded_Implementation(int32 WinnerTeamIdx)
{
	OnMatchHasEnded(WinnerTeamIdx);
}

void ASurvivalPlayerController::BeginInactiveState()
{
	Super::BeginInactiveState();

	OnDeath();
}

void ASurvivalPlayerController::EndInactiveState()
{
	Super::EndInactiveState();

	OnRespawn();
}
