// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "WeaponProjectile.h"
#include "Weapon.generated.h"

UENUM()
enum EWeaponState
{
	Idle,
	Firing,
	Reloading
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

	// [owner] Handle fire loop
	void HandleFiring();

	// [owner] Handle weapon shot
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void ShootProjectile();

	// [server] Finally shoot a projectile
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShootProjectile(FVector Origin, FVector_NetQuantizeNormal Direction);
	void ServerShootProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal Direction);
	bool ServerShootProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal Direction) { return true; };

	void StartSimulateFire();

	void StopSimulateFire();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SimulateFire();

	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
	void OnSimulateFire();

	// [client + server]
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void StartReload();

	// [server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartReload();
	void ServerStartReload_Implementation();
	bool ServerStartReload_Validate() { return true; };

	// [client + server]
	void Reload();

protected:
	// The projectile the weapon uses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class AWeaponProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Damage;

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

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Spread;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<UCameraShake> CameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0", UIMin = "0"))
	int32 MaxRoundsPerMagazine;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float NoAnimReloadDuration;

private:
	// The current weapon state
	EWeaponState CurrentState;

	// Time loop between shots. Different uses by owner and others
	FTimerHandle TimerHandle_HandleFiring;

	FTimerHandle TimerHandle_Reload;

	// Number of shots fired in one burst
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCount)
	int32 BurstCount;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	int32 CurrentRoundsInMagazine;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	bool bIsReloading;

private:
	UFUNCTION()
	void OnRep_BurstCount();

	UFUNCTION()
	void OnRep_Reload();

public:
	FORCEINLINE float GetDamage() const { return Damage; };

	FORCEINLINE int32 GetCurrentRoundsInMagazine() const { return CurrentRoundsInMagazine; };

};
