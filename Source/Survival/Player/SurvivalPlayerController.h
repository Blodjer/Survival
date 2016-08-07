// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

UCLASS()
class SURVIVAL_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerController();

	virtual void BeginPlay() override;

	virtual void InitInputSystem() override;

	virtual void SetupInputComponent() override;

	virtual void UnFreeze() override;

	UFUNCTION(BlueprintPure, Category = Spawn)
	float GetRemainingRespawnTime();

	float GetMinDieDelay();

	UFUNCTION(Client, Reliable)
	void MatchIsWaitingForPlayers();
	void MatchIsWaitingForPlayers_Implementation();

	UFUNCTION(Client, Reliable)
	void MatchHasStartedCountdown(float RemainingTime);
	void MatchHasStartedCountdown_Implementation(float RemainingTime);

	UFUNCTION(Client, Reliable)
	void MatchHasStarted();
	void MatchHasStarted_Implementation();

	UFUNCTION(Client, Reliable)
	void MatchHasEnded(int32 WinnerTeamIdx);
	void MatchHasEnded_Implementation(int32 WinnerTeamIdx);

	UFUNCTION(BlueprintCallable, Category = Pause)
	virtual void OpenPauseMenu();

	UFUNCTION(BlueprintCallable, Category = Pause)
	virtual void ClosePauseMenu();

	UFUNCTION(BlueprintPure, Category = Pause)
	virtual bool IsGameInputAllowed() const;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void SetIgnoreGameInput(bool bNewGameInput);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void ResetIgnoreGameInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual bool IsGameInputIgnored() const;

	virtual void ResetIgnoreInputFlags() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchIsWaitingForPlayers();

	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchHasStartedCountdown(float RemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchHasStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchHasEnded(int32 WinnerTeamIdx);

	void OnFadedMatchEnd();

	virtual void BeginInactiveState() override;

	virtual void EndInactiveState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnRespawn();

	UFUNCTION(BlueprintImplementableEvent, Category = Pause)
	void OnOpenPauseMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = Pause)
	void OnClosePauseMenu();

private:
	void TogglePauseMenu();

protected:
	uint8 IgnoreGameInput;

private:
	UPROPERTY(BlueprintReadOnly, Category = Pause, meta = (AllowPrivateAccess = "true"))
	bool bInPauseMenu;

	UPROPERTY(Transient)
	bool bIsWaitingForPlayers;

	FTimerHandle TimeHandle_MatchEndFade;

};
