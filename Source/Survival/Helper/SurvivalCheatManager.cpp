// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalCheatManager.h"
#include "Player/SurvivalPlayerController.h"
#include "Game/SurvivalGameState.h"
#include "Game/SurvivalGameMode.h"

void USurvivalCheatManager::SetTimeOfDay(float TimeOfDay)
{
	ASurvivalPlayerController* PlayerController = GetOuterASurvivalPlayerController();
	if (PlayerController && PlayerController->GetWorld())
	{
		ASurvivalGameState* SurvivalGameState = Cast<ASurvivalGameState>(PlayerController->GetWorld()->GetGameState());
		if (SurvivalGameState)
		{
			SurvivalGameState->SetTimeOfDay(TimeOfDay);
		}
	}
}

void USurvivalCheatManager::StartMatchCountdown()
{
	ASurvivalPlayerController* PlayerController = GetOuterASurvivalPlayerController();
	if (PlayerController && PlayerController->GetWorld())
	{
		ASurvivalGameMode* SurvivalGameMode = Cast<ASurvivalGameMode>(PlayerController->GetWorld()->GetAuthGameMode());
		if (SurvivalGameMode)
		{
			SurvivalGameMode->StartMatchCountdown();
		}
	}
}

void USurvivalCheatManager::StartMatch()
{
	ASurvivalPlayerController* PlayerController = GetOuterASurvivalPlayerController();
	if (PlayerController && PlayerController->GetWorld())
	{
		PlayerController->GetWorld()->GetAuthGameMode()->StartMatch();
	}
}

void USurvivalCheatManager::EndMatch(int32 WinnerTeamIdx)
{
	ASurvivalPlayerController* PlayerController = GetOuterASurvivalPlayerController();
	if (PlayerController && PlayerController->GetWorld())
	{
#if !UE_BUILD_SHIPPING
		ASurvivalGameMode* SurvivalGameMode = Cast<ASurvivalGameMode>(PlayerController->GetWorld()->GetAuthGameMode());
		if (SurvivalGameMode)
		{
			SurvivalGameMode->Cheat_SetMatchWinner(WinnerTeamIdx);
		}
#endif

		PlayerController->GetWorld()->GetAuthGameMode()->EndMatch();
	}
}
