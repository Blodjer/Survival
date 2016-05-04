// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/CheatManager.h"
#include "SurvivalCheatManager.generated.h"

UCLASS(Within = SurvivalPlayerController)
class SURVIVAL_API USurvivalCheatManager : public UCheatManager
{
	GENERATED_BODY()

	UFUNCTION(Exec)
	void SetTimeOfDay(float TimeOfDay);
	
};
