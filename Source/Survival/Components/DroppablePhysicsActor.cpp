// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "DroppablePhysicsActor.h"

ADroppablePhysicsActor::ADroppablePhysicsActor()
{
	bDropWithCCD = false;

	PrimaryActorTick.bCanEverTick = true;
}

void ADroppablePhysicsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && PhysicsRootComponent != nullptr && PhysicsRootComponent->IsSimulatingPhysics() && GetVelocity().Size() < 1.0f && GetWorld()->GetTimeSeconds() - CreationTime > 1.0f)
	{
		StopSimulatePhysics();
	}
}

void ADroppablePhysicsActor::OnRep_ReplicateMovement()
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

void ADroppablePhysicsActor::StartSimulatePhysics(FVector Velocity)
{
	PhysicsRootComponent = Cast<UPrimitiveComponent>(GetRootComponent());

	if (PhysicsRootComponent == nullptr)
		return;

	DefaultPhysicProfileName = PhysicsRootComponent->GetCollisionProfileName();
	
	SetReplicateMovement(true);
	PhysicsRootComponent->BodyInstance.bUseCCD = bDropWithCCD;
	PhysicsRootComponent->SetCollisionProfileName(SimulatingPhysicsProfileName.IsNone() ? DefaultPhysicProfileName : SimulatingPhysicsProfileName);
	PhysicsRootComponent->SetSimulatePhysics(true);
	PhysicsRootComponent->SetAllPhysicsLinearVelocity(Velocity);
}

void ADroppablePhysicsActor::StopSimulatePhysics()
{
	if (PhysicsRootComponent == nullptr)
		return;

	SetReplicateMovement(false);
	PhysicsRootComponent->SetCollisionProfileName(DefaultPhysicProfileName);
	PhysicsRootComponent->SetSimulatePhysics(false);
	PhysicsRootComponent->UnWeldChildren();
	if (bDropWithCCD)
	{
		PhysicsRootComponent->BodyInstance.bUseCCD = false;
	}
}
