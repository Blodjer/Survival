// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/PlayerState.h"
#include "SurvivalPlayerState.generated.h"

UCLASS()
class SURVIVAL_API ASurvivalPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerState();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Team)
	void AssignToTeam(int32 Number);

	UFUNCTION(BlueprintCallable, Category = Team)
	bool GetTeamInfo(FTeamInfo& TeamInfo);

	UFUNCTION(BlueprintCallable, Category = Team)
	TArray<ASurvivalPlayerCharacter*> GetSpawnedTeamMates();

private:
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamIdx, Category = Team, meta = (AllowPrivateAccess = "true"))
	int32 TeamIdx;

public:
	FORCEINLINE int32 GetTeamIdx() const { return TeamIdx; }

private:
	UFUNCTION()
	void OnRep_TeamIdx();

};
