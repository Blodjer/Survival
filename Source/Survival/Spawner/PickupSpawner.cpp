// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Survival/Components/PickupSpawnerComponent.h"
#include "PickupSpawner.h"

APickupSpawner::APickupSpawner()
{
	PickupSpawnerComponent = CreateDefaultSubobject<UPickupSpawnerComponent>("PickupSpawner");
	PickupSpawnerComponent->SetMobility(EComponentMobility::Static);
	RootComponent = PickupSpawnerComponent;

	bNetLoadOnClient = false;

	PrimaryActorTick.bCanEverTick = false;
}
