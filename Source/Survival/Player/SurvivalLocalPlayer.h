// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine/LocalPlayer.h"
#include "SurvivalLocalPlayer.generated.h"

UCLASS(Within = Engine, config = Engine, Transient, BlueprintType)
class SURVIVAL_API USurvivalLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Settings)
	class USurvivalUserSettings* GetSurvivalUserSettings();

private:
	UPROPERTY()
	class USurvivalUserSettings* SurvivalUserSettings;
	
};
