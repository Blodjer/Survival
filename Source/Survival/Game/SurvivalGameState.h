// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/GameState.h"
#include "SurvivalGameState.generated.h"


UCLASS()
class SURVIVAL_API ASurvivalGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASurvivalGameState();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "GameState")
	void SetMatchProperties(float LengthOfDay, float StartTimeOfDay, TArray<FTeamInfo> Teams);
	
	// Get the time since match has started in seconds
	UFUNCTION(BlueprintPure, Category = "GameState")
	float GetMatchTime();

	// Get the time of day in hours
	UFUNCTION(BlueprintPure, Category = "TimeOfDay")
	float GetTimeOfDay();

	// The length of one day in seconds
	UFUNCTION(BlueprintPure, Category = "TimeOfDay")
	float GetLengthOfDay();

	UFUNCTION(BlueprintCallable, Category = Team)
	TArray<FTeamInfo>& GetTeamInfos();

	UFUNCTION(BlueprintPure, Category = Teams)
	int32 GetNumberOfTeams();

private:
	UPROPERTY(Replicated)
	float MatchTimeStartOffset;

	UPROPERTY(Replicated)
	float GameMode_LengthOfDay;

	UPROPERTY(Replicated)
	float GameMode_StartTimeOfDay;

	UPROPERTY(Replicated)
	TArray<FTeamInfo> GameMode_Teams;
};
