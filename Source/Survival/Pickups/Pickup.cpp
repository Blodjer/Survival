// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Pickup.h"

APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionProfileName("Pickup");
	RootComponent = PickupMesh;

	SimulatingPhysicsProfileName = "PickupPhysic";
	
	PickupName = "";

	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
}

void APickup::Pickup(ASurvivalPlayerCharacter* PlayerCharacter)
{
	OnPickup(PlayerCharacter);

	Destroy();
}

void APickup::OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	// Pickup Logic
}

void APickup::SimulatePickup_Implementation()
{
	// TODO: Play pickup sfx
}
