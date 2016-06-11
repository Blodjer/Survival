// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Pickup.h"

APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionProfileName("Pickup");
	RootComponent = PickupMesh;

	PickupName = "";

	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
}

void APickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (HasAuthority() && GetRootComponent()->IsSimulatingPhysics() && GetVelocity().Size() < 1.0f && GetWorld()->GetTimeSeconds() - CreationTime > 2.0f)
	{
		StopSimulatePhysics();
	}
}

void APickup::OnRep_ReplicateMovement()
{
	Super::OnRep_ReplicateMovement();
	
	if (bReplicateMovement)
	{
		StartSimulatePhysics();
	}
	else
	{
		StopSimulatePhysics();
	}
}

void APickup::Pickup(ASurvivalPlayerCharacter* PlayerCharacter)
{
	OnPickup(PlayerCharacter);

	Destroy();
}

void APickup::StartSimulatePhysics(FVector Velocity)
{
	SetReplicateMovement(true);
	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetCollisionProfileName("PickupPhysic");
	PickupMesh->GetBodyInstance()->bUseCCD = true;
	PickupMesh->SetAllPhysicsLinearVelocity(Velocity);
}

void APickup::StopSimulatePhysics()
{
	SetReplicateMovement(false);
	PickupMesh->SetSimulatePhysics(false);
	PickupMesh->SetCollisionProfileName("Pickup");
	PickupMesh->GetBodyInstance()->bUseCCD = false;
}

void APickup::OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	// Pickup Logic
}

void APickup::SimulatePickup_Implementation()
{
	// TODO: Play pickup sfx
}
