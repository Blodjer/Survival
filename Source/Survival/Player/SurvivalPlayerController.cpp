// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"
#include "Helper/SurvivalCheatManager.h"
#include "Game/SurvivalGameMode.h"
#include "SurvivalLocalPlayer.h"
#include "Persistent/SurvivalUserSettings.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
	bInPauseMenu = false;

	CheatClass = USurvivalCheatManager::StaticClass();
}

void ASurvivalPlayerController::BeginPlay()
{
	if (!bInPauseMenu)
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(true);
		SetInputMode(InputMode);
	}
}

void ASurvivalPlayerController::InitInputSystem()
{
	Super::InitInputSystem();
	
	if (GetLocalPlayer())
	{
		USurvivalLocalPlayer* SurvivalLocalPlayer = Cast<USurvivalLocalPlayer>(GetLocalPlayer());
		if (SurvivalLocalPlayer)
		{
			SurvivalLocalPlayer->GetSurvivalUserSettings()->ApplyInputSettings();
		}
	}
}

void ASurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pause", EInputEvent::IE_Pressed, this, &ASurvivalPlayerController::TogglePauseMenu);
}

bool ASurvivalPlayerController::IsGameInputAllowed() const
{
	return !bInPauseMenu && !bCinematicMode;
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

void ASurvivalPlayerController::MatchHasStarted_Implementation()
{
	OnMatchHasStarted();
}

void ASurvivalPlayerController::MatchHasEnded_Implementation(int32 WinnerTeamIdx)
{
	OnMatchHasEnded(WinnerTeamIdx);
}

void ASurvivalPlayerController::OpenPauseMenu()
{
	if (bInPauseMenu)
		return;

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewport(false);
	SetInputMode(InputMode);

	SetCinematicMode(true, true, true);

	bInPauseMenu = true;

	if (GetPawn())
	{
		ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(GetPawn());
		if (SurvivalPlayerCharacter)
		{
			SurvivalPlayerCharacter->OnPause();
		}
	}

	OnOpenPauseMenu();
}

void ASurvivalPlayerController::ClosePauseMenu()
{
	if (!bInPauseMenu)
		return;

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
	SetInputMode(InputMode);

	SetCinematicMode(false, true, true);

	bInPauseMenu = false;

	OnClosePauseMenu();
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

void ASurvivalPlayerController::TogglePauseMenu()
{
	if (bInPauseMenu)
	{
		ClosePauseMenu();
	}
	else
	{
		OpenPauseMenu();
	}
}
