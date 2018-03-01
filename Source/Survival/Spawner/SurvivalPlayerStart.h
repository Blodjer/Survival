// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/PlayerStart.h"
#include "SurvivalPlayerStart.generated.h"

UCLASS(Abstract)
class SURVIVAL_API ASurvivalPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerStart(const FObjectInitializer& ObjectInitializer);

private:
	// Which team can start at this point
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess = "true"))
	int32 TeamIdx;

public:
	FORCEINLINE int32 GetTeamIdx() const { return TeamIdx; }

};
