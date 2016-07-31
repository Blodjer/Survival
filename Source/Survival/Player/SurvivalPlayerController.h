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

	virtual void InitInputSystem() override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintPure, Category = Pause)
	virtual bool IsGameInputAllowed() const;

	virtual void UnFreeze() override;

	UFUNCTION(BlueprintPure, Category = Spawn)
	float GetRemainingRespawnTime();

	float GetMinDieDelay();

	UFUNCTION(Client, Reliable)
	void MatchStartCountdown(float RemainingTime);
	void MatchStartCountdown_Implementation(float RemainingTime);

	UFUNCTION(Client, Reliable)
	void MatchHasEnded(int32 WinnerTeamIdx);
	void MatchHasEnded_Implementation(int32 WinnerTeamIdx);

	UFUNCTION(BlueprintCallable, Category = Pause)
	virtual void StartPause();

	UFUNCTION(BlueprintCallable, Category = Pause)
	virtual void EndPause();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchHasEnded(int32 WinnerTeamIdx);

	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchStartCountdown(float RemainingTime);

	virtual void BeginInactiveState() override;

	virtual void EndInactiveState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnRespawn();

	UFUNCTION(BlueprintImplementableEvent, Category = Pause)
	void OnPauseStart();

	UFUNCTION(BlueprintImplementableEvent, Category = Pause)
	void OnPauseEnd();

private:
	void TogglePause();

private:
	UPROPERTY(BlueprintReadOnly, Category = Pause, meta = (AllowPrivateAccess = "true"))
	bool bIsPause;

};
