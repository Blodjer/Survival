// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Survival/Components/PickupSpawnerComponent.h"
#include "PickupSpawner.h"

APickupSpawner::APickupSpawner()
{
	PickupSpawnerComponent = CreateDefaultSubobject<UPickupSpawnerComponent>("PickupSpawner");
	RootComponent = PickupSpawnerComponent;

	PrimaryActorTick.bCanEverTick = false;
}
