// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Pickup.h"

APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionProfileName("Pickup");
	PickupMesh->bGenerateOverlapEvents = false;
	RootComponent = PickupMesh;

	SimulatingPhysicsProfileName = "PickupPhysic";
	bDropWithCCD = true;

	PickupName = "";

	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;
}

void APickup::Interact_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	Pickup(PlayerCharacter);
}

bool APickup::IsInteractable_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	return PlayerCharacter ? PlayerCharacter->CanPickup(this) : false;
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
