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

public:
	void Killed(AController* Killer, AController* KilledPlayer, const UDamageType* DamageType);

protected:
	// The length of one day in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = "TimeOfDay")
	float LengthOfDay;

	// At what time the match should start (0-24)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "24.0", UIMin = "0.0", UIMax = "24.0"), Category = "TimeOfDay")
	float StartTimeOfDay;
	
};
