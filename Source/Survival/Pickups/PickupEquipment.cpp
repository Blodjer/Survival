// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "PickupEquipment.h"

void APickupEquipment::OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	if (HandheldClass != nullptr)
	{
		PlayerCharacter->SpawnHandheld(HandheldClass);
	}

	Super::OnPickup_Implementation(PlayerCharacter);
}
