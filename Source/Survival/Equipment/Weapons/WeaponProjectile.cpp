// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "WeaponProjectile.h"
#include "Weapon.h"


AWeaponProjectile::AWeaponProjectile()
{
	// Create the sphere collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(2.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->bTraceComplexOnMove = true;
	RootComponent = CollisionComponent;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystem");
	ParticleSystem->bAutoActivate = true;
	ParticleSystem->AttachTo(RootComponent);

	// Create the projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	Speed = 900.0f;
	LifeSpan = 4.0f;

	// Update dependent variables
	ProjectileMovement->InitialSpeed = Speed * 100.0f;
	ProjectileMovement->MaxSpeed = Speed * 100.0f;
	InitialLifeSpan = LifeSpan;

	bReplicates = true;
	bReplicateMovement = true;

	PrimaryActorTick.bCanEverTick = true;
}

#if WITH_EDITOR
void AWeaponProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Update dependent variables
	ProjectileMovement->InitialSpeed = Speed * 100.0f;
	ProjectileMovement->MaxSpeed = Speed * 100.0f;
	InitialLifeSpan = LifeSpan;
}
#endif

void AWeaponProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Ignore owner character and weapon collision
	CollisionComponent->MoveIgnoreActors.Add(GetInstigator());
	CollisionComponent->MoveIgnoreActors.Add(GetOwner());

	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AWeaponProjectile::OnImpact);

	if (GetOwner())
	{
		AWeapon* OwnerWeapon = Cast<AWeapon>(GetOwner());
		if (OwnerWeapon)
		{
			Damage = OwnerWeapon->GetDamage();
		}
	}
}

void AWeaponProjectile::InitProjectile(FVector& Direction)
{
	// Set the initial velocity
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void AWeaponProjectile::OnImpact(const FHitResult& HitResult)
{
	if (HasAuthority() && HitResult.GetActor() != nullptr)
	{
			FPointDamageEvent PointDamage = FPointDamageEvent();
			PointDamage.Damage = Damage;
			PointDamage.HitInfo = HitResult;
			PointDamage.ShotDirection = HitResult.ImpactNormal;

			HitResult.GetActor()->TakeDamage(Damage, PointDamage, GetInstigatorController(), this);
	}

	//Destroy(true);
	SetLifeSpan(0.5f);
}
