// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "DroppablePhysicsActor.h"

ADroppablePhysicsActor::ADroppablePhysicsActor()
{

	PrimaryActorTick.bCanEverTick = true;
}

void ADroppablePhysicsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && PhysicsRootComponent != nullptr && PhysicsRootComponent->IsSimulatingPhysics() && GetVelocity().Size() < 1.0f && GetWorld()->GetTimeSeconds() - CreationTime > 2.0f)
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
	PhysicsRootComponent->SetSimulatePhysics(true);
	PhysicsRootComponent->SetCollisionProfileName(SimulatingPhysicsProfileName);
	PhysicsRootComponent->GetBodyInstance()->bUseCCD = true;
	PhysicsRootComponent->SetAllPhysicsLinearVelocity(Velocity);
}

void ADroppablePhysicsActor::StopSimulatePhysics()
{
	if (PhysicsRootComponent == nullptr)
		return;

	SetReplicateMovement(false);
	PhysicsRootComponent->SetSimulatePhysics(false);
	PhysicsRootComponent->SetCollisionProfileName(DefaultPhysicProfileName);
	PhysicsRootComponent->GetBodyInstance()->bUseCCD = false;
}
