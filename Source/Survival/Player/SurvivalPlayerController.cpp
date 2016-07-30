// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"
#include "Helper/SurvivalCheatManager.h"
#include "Game/SurvivalGameMode.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
	bIsPause = false;

	CheatClass = USurvivalCheatManager::StaticClass();
}

void ASurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pause", EInputEvent::IE_Pressed, this, &ASurvivalPlayerController::TogglePause);
}

bool ASurvivalPlayerController::IsGameInputAllowed() const
{
	return !bIsPause && !bCinematicMode;
}

void ASurvivalPlayerController::UnFreeze()
{
	Super::UnFreeze();

	if (GetWorld() && GetWorld()->GetAuthGameMode() && !GetWorld()->GetAuthGameMode()->HasMatchStarted())
	{
		GetWorld()->GetAuthGameMode()->RestartPlayer(this);
	}
	else
	{
		ServerRestartPlayer();
	}
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

void ASurvivalPlayerController::StartPause()
{
	if (bIsPause)
		return;

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewport(false);
	SetInputMode(InputMode);

	SetCinematicMode(true, true, true);

	bIsPause = true;

	if (GetPawn())
	{
		ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(GetPawn());
		if (SurvivalPlayerCharacter)
		{
			SurvivalPlayerCharacter->OnPause();
		}
	}

	OnPauseStart();
}

void ASurvivalPlayerController::EndPause()
{
	if (!bIsPause)
		return;

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
	SetInputMode(InputMode);

	SetCinematicMode(false, true, true);

	bIsPause = false;

	OnPauseEnd();
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

void ASurvivalPlayerController::TogglePause()
{
	if (bIsPause)
	{
		EndPause();
	}
	else
	{
		StartPause();
	}
}
