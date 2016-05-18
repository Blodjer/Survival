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

	virtual void UnFreeze() override;

	UFUNCTION(BlueprintPure, Category = Spawn)
	float GetRemainingRespawnTime();

	float GetMinDieDelay();

	UFUNCTION(Client, Reliable)
	void MatchHasEnded(int32 WinnerTeamIdx);
	void MatchHasEnded_Implementation(int32 WinnerTeamIdx);

	UFUNCTION(BlueprintImplementableEvent, Category = Match)
	void OnMatchHasEnded(int32 WinnerTeamIdx);

protected:
	virtual void BeginInactiveState() override;

	virtual void EndInactiveState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
	void OnRespawn();

};
