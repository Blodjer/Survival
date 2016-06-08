// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "WeaponProjectile.generated.h"

UCLASS(Abstract)
class SURVIVAL_API AWeaponProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponProjectile();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitializeComponents() override;

	// Initialize the projectile
	virtual void InitProjectile(FVector& Direction);

private:
	// Handle hit
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

protected:
	// Projectile speed in m/s
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float Speed;

	// How long this Projectiles lives before destroying (0 = forever)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float LifeSpan;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Projectile)
	float Damage;

private:
	// Projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	// Projectile sphere collision. Used by movement component for update
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ParticleSystem;

};
