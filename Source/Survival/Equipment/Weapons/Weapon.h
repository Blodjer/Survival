// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "WeaponProjectile.h"
#include "Weapon.generated.h"

UENUM()
enum EWeaponState
{
	Idle,
	Firing
};

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AWeapon : public AHandheld
{
	GENERATED_BODY()
	
public:	
	AWeapon();

protected:
	virtual void SetupInputActions() override;

	// [client + server]
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void StartFire();

	// [server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate() { return true; };

	// [client + server]
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void StopFire();

	// [server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate() { return true; };

	// [client] Handle fire loop
	void HandleFiring();

	// [server] Finally shoot a projectil
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShootProjectile(FVector Origin, FVector_NetQuantizeNormal Direction);
	void ServerShootProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal Direction);
	bool ServerShootProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal Direction) { return true; };

	void StartSimulateFire();

	void StopSimulateFire();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SimulateFire();

protected:
	// The projectile the weapon uses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class AWeaponProjectile> ProjectileClass;

	// Rate of fire in rpm (rounds per minute)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RecoilUpMin;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RecoilUpMax;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RecoilLeft;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RecoilRight;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<UCameraShake> CameraShake;

private:
	// The current weapon state
	EWeaponState CurrentState;

	// Time loop between shots. Different uses by owner and others
	FTimerHandle TimerHandle_HandleFiring;

	// Number of shots fired in one burst
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCount)
	int32 BurstCount;

private:
	UFUNCTION()
	void OnRep_BurstCount();

};
