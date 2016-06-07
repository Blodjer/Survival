// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Pickups/Pickup.h"
#include "Survival/Equipment/Handheld.h"
#include "PickupEquipment.generated.h"

UCLASS()
class SURVIVAL_API APickupEquipment : public APickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pickup)
	TSubclassOf<AHandheld> HandheldClass;

protected:
	virtual void OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter) override;
	
};
