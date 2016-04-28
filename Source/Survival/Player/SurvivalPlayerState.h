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

private:
	UPROPERTY(Transient, BlueprintReadOnly, Replicated, Category = Team, meta = (AllowPrivateAccess = "true"))
	int32 TeamNumber;

public:
	FORCEINLINE int32 GetTeamNumber() const { return TeamNumber; }

};
