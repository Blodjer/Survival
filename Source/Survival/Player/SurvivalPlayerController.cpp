// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerController.h"
#include "Helper/SurvivalCheatManager.h"
#include "Game/SurvivalGameMode.h"
#include "SurvivalLocalPlayer.h"
#include "Persistent/SurvivalUserSettings.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
	IgnoreGameInput = 0;

	bInPauseMenu = false;

	CheatClass = USurvivalCheatManager::StaticClass();
}

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

void ASurvivalPlayerController::UnFreeze()
{
	Super::UnFreeze();

	if (GetWorld() == nullptr || GetWorld()->GetAuthGameMode() == nullptr)
		return;

	if (GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::WaitingToStart)
	{
		GetWorld()->GetAuthGameMode()->RestartPlayer(this);
	}
	else if (GetWorld()->GetAuthGameMode()->IsMatchInProgress())
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

void ASurvivalPlayerController::MatchIsWaitingForPlayers_Implementation()
{
	SetIgnoreMoveInput(true);
	SetIgnoreGameInput(true);

	bPlayerIsWaiting = true;

	OnMatchIsWaitingForPlayers();
}

void ASurvivalPlayerController::MatchHasStartedCountdown_Implementation(float RemainingTime)
{
	if (bPlayerIsWaiting)
	{
		SetIgnoreMoveInput(false);
		SetIgnoreGameInput(false);

		bPlayerIsWaiting = false;
	}

	OnMatchHasStartedCountdown(RemainingTime);
}

void ASurvivalPlayerController::MatchHasStarted_Implementation()
{
	SetIgnoreMoveInput(false);
	SetIgnoreGameInput(false);

	OnMatchHasStarted();
}

void ASurvivalPlayerController::MatchHasEnded_Implementation(int32 WinnerTeamIdx)
{
	FlushPressedKeys();
	StopMovement();

	if (GetPawn() && GetPawn()->GetMovementComponent())
	{
		GetPawn()->GetMovementComponent()->StopMovementImmediately();
	}

	OnMatchHasEnded(WinnerTeamIdx);

	UnPossess();

	GetWorldTimerManager().ClearTimer(TimerHandle_UnFreeze);
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
			SurvivalPlayerCharacter->OnOpenPauseMenu();
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

bool ASurvivalPlayerController::IsGameInputAllowed() const
{
	return !bInPauseMenu && !bCinematicMode && !IsGameInputIgnored();
}

void ASurvivalPlayerController::SetIgnoreGameInput(bool bNewGameInput)
{
	IgnoreGameInput = FMath::Max(IgnoreGameInput + (bNewGameInput ? +1 : -1), 0);
}

void ASurvivalPlayerController::ResetIgnoreGameInput()
{
	IgnoreGameInput = 0;
}

bool ASurvivalPlayerController::IsGameInputIgnored() const
{
	return (IgnoreGameInput > 0);
}

void ASurvivalPlayerController::ResetIgnoreInputFlags()
{
	if (!bPlayerIsWaiting)
	{
		Super::ResetIgnoreInputFlags();

		ResetIgnoreGameInput();
	}
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
