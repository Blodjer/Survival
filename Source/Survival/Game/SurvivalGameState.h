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

	UFUNCTION(BlueprintCallable, Category = Team)
	FTeamInfo& GetTeamInfo(int32 TeamIdx);

	UFUNCTION(BlueprintPure, Category = Teams)
	int32 GetNumberOfTeams();

	void SetTimeOfDay(float TimeOfDay);

	UFUNCTION(BlueprintPure, Category = Math)
	int32 GetReplicatedSeed() const;

private:
	UPROPERTY(Replicated, Transient)
	float MatchTimeStartOffset;

	UPROPERTY(Replicated, Transient)
	float GameMode_LengthOfDay;

	UPROPERTY(Replicated, Transient)
	float GameMode_StartTimeOfDay;

	UPROPERTY(Replicated, Transient)
	TArray<FTeamInfo> GameMode_Teams;

	UPROPERTY(Replicated)
	int32 Seed;
};
