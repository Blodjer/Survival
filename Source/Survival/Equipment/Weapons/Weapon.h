// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "WeaponProjectile.h"
#include "Survival/Player/SurvivalCharacterMovement.h"
#include "Weapon.generated.h"

UENUM()
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Automatic,
	Burst,
	SemiAutomatic
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FWeaponMovementValues : public FCharacterMovementValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float StandingIdle_ADS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float StandingMove_ADS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float CrouchingIdle_ADS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float CrouchingMove_ADS;

	virtual float GetValue(ACharacter* Character) const override
	{
		if (Character == nullptr)
			return StandingIdle_ADS;

		float value = 0.0f;

		if (false) // bIsADS
		{
			bool bIsMoving = !Character->GetVelocity().IsNearlyZero(0.5f);
			if (Character->bIsCrouched)
				value = bIsMoving ? CrouchingMove_ADS : CrouchingIdle_ADS;
			else
				value = bIsMoving ? StandingMove_ADS : StandingIdle_ADS;
		}
		else
		{
			value = FCharacterMovementValues::GetValue(Character);
		}

		return value;
	}
};

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AWeapon : public AHandheld
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void UnEquip() override;

	virtual void OnCharacterStopUse() override;

protected:
	virtual void SetupInputActions() override;

	virtual void BeforeDrop() override;

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

	UFUNCTION(BlueprintPure, Category = Weapon)
	virtual bool CanFire();

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

	void SwitchFireMode();

	// [server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwitchFireMode();
	void ServerSwitchFireMode_Implementation();
	bool ServerSwitchFireMode_Validate() { return true; };

	void SimulateSwitchFireMode();

	UFUNCTION(BlueprintPure, Category = Weapon)
	EFireMode GetBestFireMode();

	bool IsValidFireMode(EFireMode FireMode);

protected:
	// The projectile the weapon uses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class AWeaponProjectile> ProjectileType;

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
	float RecoilFirstShotMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin = "0.0", UIMin = "0.0"))
	UDataTable* SpreadDataTable;

	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetSpreadBase() const;

	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetSpreadMax() const;

	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetSpreadIncrease() const;

	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetSpreadDecrease() const;

	UPROPERTY(EditDefaultsOnly, Category = FireModes)
	bool bAutomatic;
	UPROPERTY(EditDefaultsOnly, Category = FireModes)
	bool bBurst;
	UPROPERTY(EditDefaultsOnly, Category = FireModes)
	bool bSemiAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<UCameraShake> CameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0", UIMin = "0"))
	int32 MaxRoundsPerMagazine;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float NoAnimReloadDuration;

private:
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

	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_FireMode, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	EFireMode FireMode;

	UPROPERTY(Transient)
	float LastShotTime;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	float CurrentSpread;

private:
	UFUNCTION()
	void OnRep_BurstCount();

	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION()
	void OnRep_FireMode();

public:
	FORCEINLINE float GetDamage() const { return Damage; };

	FORCEINLINE int32 GetCurrentRoundsInMagazine() const { return CurrentRoundsInMagazine; };

};
