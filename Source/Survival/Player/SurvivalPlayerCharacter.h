// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Character.h"
#include "SurvivalCharacterMovement.h"
#include "SurvivalPlayerController.h"
#include "Survival/Equipment/Weapons/WeaponProjectile.h"
#include "SurvivalPlayerCharacter.generated.h"

UCLASS()
class SURVIVAL_API ASurvivalPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivalPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	
	virtual void OnRep_Controller() override;

	virtual void OnUpdateTeamInfo();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void Landed(const FHitResult& Hit) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Character)
	void Die(const FDamageEvent& DamageEvent, AController* Killer, bool bImmediately = true);

private:
	void Die();

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Character)
	void Revive(float NewHealth);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Character)
	void Heal(float Value);

	UFUNCTION(BlueprintCallable, Category = Team)
	bool GetTeamInfo(FTeamInfo& TeamInfo);

	UFUNCTION(BlueprintCallable, Category = Team)
	int32 GetTeamIdx();

	UFUNCTION(BlueprintPure, Category = Equipment)
	class AWeapon* GetEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	void AddAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 RequestAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 GetAmmoAmmountOfType(TSubclassOf<class AWeaponProjectile> Type) const;

	UFUNCTION(BlueprintCallable, Category = Battery)
	void AddBatteryPower(float Amount);

	UFUNCTION(BlueprintCallable, Category = Battery)
	void DrainBatteryPower(float Amount);

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire)
	class ACampfire* CapturingCampfire;

	UPROPERTY(BlueprintReadOnly, Replicated, Transient, Category = Ammunition)
	FAmmunitionInventory AmmunitionInventory;

	UPROPERTY(BlueprintReadOnly, Replicated, Transient, Category = Battery)
	float CurrentBatteryPower;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Character, meta = (AllowPrivateAccess = "true"))
	void OnPossessedLocal();

	UFUNCTION(BlueprintCallable, Category = Character)
	void SetRagdollPhysics();

private:
	// Moves the character alongs his x-axis
	void MoveForward(float Value);

	// Moves the character along his y-axis
	void MoveRight(float Value);

	// Turns the character around his z-axis
	void Turn(float Value);

	// Rotates the camera up
	void LookUp(float Value);

	// Turns the character around his z-axis at BaseTurnRate
	void TurnAtRate(float Value);

	// Rotates the camera up at BaseLookUpRate
	void LookUpAtRate(float Value);

	// Start sprinting
	UFUNCTION(BlueprintCallable, Category = Sprint)
	void StartSprint();

	// Stop sprinting
	UFUNCTION(BlueprintCallable, Category = Sprint)
	void StopSprint();
	
	// Start or Stop sprinting
	void SetSprint(bool bShouldSprint);

	// Starts or stops sprinting on server. Replicates to all clients except owner
	UFUNCTION(Server, Reliable, WithValidation, Category = Sprint)
	void ServerSetSprint(bool bShouldSprint);
	void ServerSetSprint_Implementation(bool bShouldSprint);
	bool ServerSetSprint_Validate(bool bShouldSprint) { return true; };

	// Toggle crouching
	UFUNCTION(BlueprintCallable, Category = Crouch)
	void ToggleCrouch();

	// Starts crouchin (forwarding to character movement component)
	void StartCrouch();

	// Stops crouchin (forwarding to character movement component)
	void StopCrouch();

	// Toggle the light visibility on/off
	UFUNCTION(BlueprintCallable, Category = Flashlight)
	void ToggleFlashlight();

	// Set the light on or off. Replicates to server
	UFUNCTION(BlueprintCallable, Category = Flashlight)
	void SetFlashlightOn(bool bOn);

	// Set the light on or off on the server. Replicate to all clients except the owner
	UFUNCTION(Server, Reliable, WithValidation, Category = Flashlight)
	void ServerSetFlashlightOn(bool bOn);
	void ServerSetFlashlightOn_Implementation(bool bOn);
	bool ServerSetFlashlightOn_Validate(bool bOn) { return true; };

	// Spawn a specific hendheld. Handled by server. TODO: Replace with Spawn Loadout/Equipment
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = Equipment)
	void SpawnHandheld(TSubclassOf<class AHandheld> HandheldClass);
	void SpawnHandheld_Implementation(TSubclassOf<class AHandheld> HandheldClass);
	bool SpawnHandheld_Validate(TSubclassOf<class AHandheld> HandheldClass) { return true; };

	// Equip a hendheld actor
	UFUNCTION(BlueprintCallable, Category = Equipment)
	void Equip(AHandheld* Handheld);

	UFUNCTION(Server, Reliable, WithValidation, Category = Equipment)
	void ServerEquip(AHandheld* Handheld);
	void ServerEquip_Implementation(AHandheld* Handheld);
	bool ServerEquip_Validate(AHandheld* Handheld) { return true; };

	UFUNCTION()
	void SimulateEquip(AHandheld* Handheld);

	void UpdateTeamColors();

	UFUNCTION(BlueprintCallable, Category = Pickup)
	void UpdateTargetPickup();

	void Pickup();

	UFUNCTION(Server, Reliable, WithValidation, Category = Pickup)
	void ServerPickup(class APickup* Pickup);
	void ServerPickup_Implementation(class APickup* Pickup);
	bool ServerPickup_Validate(class APickup* Pickup) { return true; };

private:
	// First person mesh. Seen only by owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	
	// Spotlight used as flashlight. Attached to view.
	UPROPERTY(VisibleAnywhere, Category = Flashlight)
	class USpotLightComponent* Flashlight;

	// SurvivalCharacterMovementComponent (extends CharacterMovementComponent)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USurvivalCharacterMovement* SurvivalCharacterMovement;

	// Is the flashlight turned on or off
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsFlashlightOn)
	bool bIsFlashlightOn;

	UPROPERTY(Transient, Replicated)
	bool bIsDying;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead;

	FTimerHandle TimerHandle_Die;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	class APickup* TargetingPickup;

protected:
	// Base controller turn rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	// Base controller look up/down rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// Current health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Health;

	// Landing velocity at which the character starts taking damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DamagingLandingVelocity;

	// Landing velocity at which the character dies
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DeadlyLandingVelocity;

	// Set by character movement to specify that this Character is currently sprinting
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_IsSprinting, Category = Character)
	bool bIsSprinting;

	// The currently equipped handheld
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_EquippedHandheld, Category = Equipment)
	class AHandheld* EquippedHandheld;

	// Socket name for attaching handheld meshes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment)
	FName HandheldAttachPoint;

	// Default equipment spawned on player spawn
	UPROPERTY(EditDefaultsOnly, Category = Loadout)
	TArray<TSubclassOf<class AHandheld>> StartEquipment;
	
	// Default amount of ammo on player spawn
	UPROPERTY(EditDefaultsOnly, Category = Loadout)
	TArray<FAmmo> StartAmmunition;

	// Default battery power player spawn
	UPROPERTY(EditDefaultsOnly, Category = Loadout, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float StartBatteryPower;

	// Battery power drain per second
	UPROPERTY(EditDefaultsOnly, Category = Flashlight, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float FlashlightBatteryPowerDrain;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pickup)
	float PickupRange;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Material)
	TArray<UMaterialInstanceDynamic*> MeshMIDs;

public:
	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	// Return the camera component
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	// Return the custom character movement
	FORCEINLINE USurvivalCharacterMovement* GetCharacterMovement() const { return SurvivalCharacterMovement; }

	FORCEINLINE ASurvivalPlayerController* GetPlayerController() const { return GetController() ? Cast<ASurvivalPlayerController>(GetController()) : nullptr; }

	// Return the socket name for attaching handheld meshes
	FORCEINLINE FName GetHandheldAttachPoint() const { return HandheldAttachPoint; }

	// The health value the player starts with
	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE float GetMaxHealth() const { return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Health; };

	// The health value the player starts with
	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE float GetMaxFlashlightIntensity() const { return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Flashlight->Intensity; };
	
private:
	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION()
	void OnRep_IsFlashlightOn();
	
	UFUNCTION()
	void OnRep_EquippedHandheld();

	UFUNCTION()
	void OnRep_IsDead();

};
