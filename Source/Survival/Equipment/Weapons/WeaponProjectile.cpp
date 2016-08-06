// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "WeaponProjectile.h"
#include "Weapon.h"


AWeaponProjectile::AWeaponProjectile()
{
	// Create the sphere collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(1.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->bTraceComplexOnMove = true;
	CollisionComponent->bReturnMaterialOnMove = true;
	CollisionComponent->AlwaysLoadOnServer = true;
	CollisionComponent->AlwaysLoadOnClient = true;
	RootComponent = CollisionComponent;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystem");
	ParticleSystem->bAutoActivate = true;
	ParticleSystem->SetupAttachment(RootComponent);

	// Create the projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	Speed = 750.0f;
	LifeSpan = 4.0f;

	// Update dependent variables
	ProjectileMovement->InitialSpeed = Speed * 100.0f;
	ProjectileMovement->MaxSpeed = Speed * 100.0f;
	InitialLifeSpan = LifeSpan;

	ProjectileName = "Unkown";

	bReplicates = false;
	bNetTemporary = true;

	PrimaryActorTick.bCanEverTick = false;
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
	ProjectileMovement->bInitialVelocityInLocalSpace = false;
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void AWeaponProjectile::OnImpact(const FHitResult& HitResult)
{
	if (HasAuthority() && GetWorld()->IsServer() && HitResult.GetActor() != nullptr)
	{
		FPointDamageEvent PointDamage = FPointDamageEvent();
		PointDamage.Damage = Damage;
		PointDamage.HitInfo = HitResult;
		PointDamage.ShotDirection = HitResult.ImpactNormal;

		HitResult.GetActor()->TakeDamage(Damage, PointDamage, GetInstigatorController(), this);
	}

	UPhysicalMaterial* HitPhysicalMaterial = HitResult.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysicalMaterial);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetImpactVFX(HitSurfaceType), HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), true);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), GetImpactSFX(HitSurfaceType), HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

	Destroy();
}

UParticleSystem* AWeaponProjectile::GetImpactVFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ParticleSystem = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_Default:	ParticleSystem = ImpactVFX.Default; break;
		case SURFACE_Wood:		ParticleSystem = ImpactVFX.Wood; break;
		case SURFACE_Stone:		ParticleSystem = ImpactVFX.Stone; break;
		case SURFACE_Dirt:		ParticleSystem = ImpactVFX.Dirt; break;
		case SURFACE_Grass:		ParticleSystem = ImpactVFX.Grass; break;
		case SURFACE_Water:		ParticleSystem = ImpactVFX.Water; break;
		case SURFACE_Flesh:		ParticleSystem = ImpactVFX.Flesh; break;
		case SURFACE_Metal:		ParticleSystem = ImpactVFX.Metal; break;
		default:				ParticleSystem = ImpactVFX.Default; break;
	}

	return ParticleSystem;
}

USoundBase* AWeaponProjectile::GetImpactSFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundBase* Sound = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_Default:	Sound = ImpactSFX.Default; break;
		case SURFACE_Wood:		Sound = ImpactSFX.Wood; break;
		case SURFACE_Stone:		Sound = ImpactSFX.Stone; break;
		case SURFACE_Dirt:		Sound = ImpactSFX.Dirt; break;
		case SURFACE_Grass:		Sound = ImpactSFX.Grass; break;
		case SURFACE_Water:		Sound = ImpactSFX.Water; break;
		case SURFACE_Flesh:		Sound = ImpactSFX.Flesh; break;
		case SURFACE_Metal:		Sound = ImpactSFX.Metal; break;
		default:				Sound = ImpactSFX.Default; break;
	}

	return Sound;
}
