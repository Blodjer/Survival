// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "WeaponProjectile.h"


AWeaponProjectile::AWeaponProjectile()
{
	// Create the sphere collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(3.0f);
	CollisionComponent->SetCollisionProfileName("BlockAllDynamic");
	CollisionComponent->bTraceComplexOnMove = true;
	RootComponent = CollisionComponent;

	// Create the projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	Speed = 900.0f;
	LifeSpan = 4.0f;

	bReplicates = true;
	bReplicateMovement = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Update dependent variables
	ProjectileMovement->InitialSpeed = Speed * 100.0f;
	ProjectileMovement->MaxSpeed = Speed * 100.0f;

	InitialLifeSpan = LifeSpan;
}

void AWeaponProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Ignore owner character and weapon collision
	CollisionComponent->MoveIgnoreActors.Add(GetInstigator());
	CollisionComponent->MoveIgnoreActors.Add(GetOwner());
}

void AWeaponProjectile::InitProjectile(FVector& Direction)
{
	// Set the initial velocity
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}
