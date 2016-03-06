// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/GameMode.h"
#include "SurvivalGameMode.generated.h"


UCLASS()
class SURVIVAL_API ASurvivalGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ASurvivalGameMode();

	virtual void InitGameState() override;

protected:
	// The length of one day in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TimeOfDay")
	float LengthOfDay;
	
};
