// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/PlayerStart.h"
#include "SurvivalPlayerStart.generated.h"

UCLASS(Abstract)
class SURVIVAL_API ASurvivalPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerStart();

private:
	// Which team can start at this point
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess = "true"))
	int32 TeamNumber;

public:
	FORCEINLINE int32 GetTeamNumber() const { return TeamNumber; }

};
