// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Character.h"
#include "SurvivalCharacterMovement.h"
#include "SurvivalPlayerController.h"
#include "Survival/Equipment/Handheld.h"
#include "Survival/Equipment/Weapons/WeaponProjectile.h"
#include "SurvivalPlayerCharacter.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FHandheldInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, NotReplicated, Category = Slot)
	EHandheldType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, NotReplicated, Category = Slot)
	FName SocketName;

	UPROPERTY(Transient)
	AHandheld* AssignedHandheld;

	FHandheldInventorySlot() : Type(EHandheldType::Unknown), SocketName(""), AssignedHandheld(nullptr)
	{
		
	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FHandheldInventorySlotManager
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<FHandheldInventorySlot> Slots;

	FName Store(AHandheld* Handheld)
	{
		for (FHandheldInventorySlot& Slot : Slots)
		{
			if (Slot.AssignedHandheld == nullptr && Slot.Type == Handheld->GetHandheldType())
			{
				if (Handheld->HasAuthority())
				{
					Slot.AssignedHandheld = Handheld;
				}

				return Slot.SocketName;
			}
		}

		return "";
	}

	void Take(AHandheld* Handheld)
	{
		for (FHandheldInventorySlot& Slot : Slots)
		{
			if (Slot.AssignedHandheld == Handheld)
			{
				if (Handheld->HasAuthority())
				{
					Slot.AssignedHandheld = nullptr;
				}
			}
		}
	}

	FName GetSocketName(AHandheld* Handheld)
	{
		for (FHandheldInventorySlot Slot : Slots)
		{
			if (Slot.AssignedHandheld == Handheld)
			{
				return Slot.SocketName;
			}
		}

		return "";
	}

	int32 GetAvailableSlots(EHandheldType Type)
	{
		int32 i = 0;
		for (FHandheldInventorySlot Slot : Slots)
		{
			if (Slot.Type == Type && Slot.AssignedHandheld == nullptr)
			{
				i++;
			}
		}
		return i;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConfirmDamageSignature, float, Damage, FDamageEvent const&, DamageEvent);

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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage = nullptr) override;

	virtual void StopAllAnimMontages();

public:
	virtual void Landed(const FHitResult& Hit) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Character)
	void Die(const FDamageEvent& DamageEvent, AController* Killer, bool bImmediately = true);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Character)
	void InjureLethal();

private:
	void Die();

	UFUNCTION(Client, Reliable, Category = "Game|Damage")
	void ConfirmDamage(float Damage, FDamageEvent const& DamageEvent);
	void ConfirmDamage_Implementation(float Damage, FDamageEvent const& DamageEvent);

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
	FORCEINLINE class AHandheld* GetEquippedHandheld() const { return EquippedHandheld; };

	UFUNCTION(BlueprintPure, Category = Equipment)
	class AWeapon* GetEquippedWeapon() const;

	// Spawn a specific hendheld. Handled by server
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	void AddHandheldToInventory(TSubclassOf<class AHandheld> HandheldClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	void RemoveHandheldFromInventory(class AHandheld* Handheld);

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 AddAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount, bool bOverfill = false);

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 RequestAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 GetAmmoAmountOfType(TSubclassOf<class AWeaponProjectile> Type) const;

	UFUNCTION(BlueprintCallable, Category = Ammunition)
	int32 GetRemainingAmmoAmountOfType(TSubclassOf<class AWeaponProjectile> Type) const;

	UFUNCTION(BlueprintPure, Category = Interactable)
	const TScriptInterface<IInteractable> GetTargetingInteractableInterface() const;

	const TScriptInterface<IInteractable> GetInteractableInterface(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = Pickup)
	bool CanPickup(APickup* Pickup);

	UFUNCTION(BlueprintCallable, Category = Battery)
	void AddBatteryPower(float Amount);

	UFUNCTION(BlueprintCallable, Category = Battery)
	void DrainBatteryPower(float Amount);

	UFUNCTION(BlueprintPure, Category = Character)
	float GetDeadness() const;

	UFUNCTION(BlueprintPure, Category = Character)
	bool IsGameInputAllowed() const;

	void OnOpenPauseMenu();

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire)
	class ACampfire* CapturingCampfire;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HandheldInventory, Transient, Category = Equipment)
	TArray<class AHandheld*> HandheldInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Equipment)
	FHandheldInventorySlotManager HandheldInventorySlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Transient, Category = Equipment)
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

	virtual void Jump() override;

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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	void DestroyInventory();

	// Equip a hendheld actor
	UFUNCTION(BlueprintCallable, Category = Equipment)
	void Equip(AHandheld* Handheld);

	UFUNCTION(Server, Reliable, WithValidation, Category = Equipment)
	void ServerEquip(AHandheld* Handheld);
	void ServerEquip_Implementation(AHandheld* Handheld);
	bool ServerEquip_Validate(AHandheld* Handheld) { return true; };

	void SimulateEquip(AHandheld* Handheld);

	void SimulateUnEquip(AHandheld* Handheld);

	void NextHandheld();

	void PreviousHandheld();

	void EquipSlot(int32 SlotIndex);

	template<int32 SlotIndex>
	void EquipSlot()
	{
		if (IsGameInputAllowed())
		{
			EquipSlot(SlotIndex);
		}
	}

	void UpdateTeamColors();

	UFUNCTION(BlueprintCallable, Category = Interactable)
	void UpdateTargetInteractable();

	void Interact();

	UFUNCTION(Server, Reliable, WithValidation, Category = Interactable)
	void ServerInteract(class AActor* Interactable);
	void ServerInteract_Implementation(class AActor* Interactable);
	bool ServerInteract_Validate(class AActor* Interactable) { return true; };

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	void DropHandheld();

	UFUNCTION(Server, Reliable, WithValidation, Category = Equipment)
	void ServerDropHandheld();
	void ServerDropHandheld_Implementation();
	bool ServerDropHandheld_Validate() { return true; };

	UFUNCTION(BlueprintCallable, Category = Character)
	void UpdateSurface();

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

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsDying)
	bool bIsLethalInjured;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead;

	UPROPERTY(Transient)
	class AHandheld* PreviousEquippedHandheld;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Interactable, meta = (AllowPrivateAccess = "true"))
	class AActor* TargetingInteractableActor;

	UPROPERTY(Transient)
	TEnumAsByte<EPhysicalSurface> CurrentSurface;

private:
	FTimerHandle TimerHandle_Die;

	FTimerHandle TimerHandle_UpdateSurface;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character)
	TArray<FHitZone> HitZones;

	// Landing velocity at which the character starts taking damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DamagingLandingVelocity;

	// Landing velocity at which the character dies
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DeadlyLandingVelocity;

	// Set by character movement to specify that this Character is currently sprinting
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Stamina;

	UPROPERTY(EditDefaultsOnly, Category = Character, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaDecrease;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	float StaminaDecreaseOffset;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FCharacterMovementValues StaminaIncrease;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interactable)
	float InteractionRange;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	float UpdateSurfaceRate;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = Material)
	TArray<UMaterialInstanceDynamic*> MeshMIDs;

public:
	UPROPERTY(BlueprintAssignable, Category = "Game|Damage")
	FConfirmDamageSignature OnConfirmDamage;

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

	// Is the player lethal injured and will die soon
	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE bool IsLethalInjured() const { return bIsLethalInjured; };

	// Is the player currently sprinting
	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE bool IsSprinting() const { return bIsSprinting && GetVelocity() != FVector::ZeroVector; };

	FORCEINLINE float GetStamina() const { return Stamina; };

	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE float GetMaxStamina() const { return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Stamina; };

	// The health value the player starts with
	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE float GetMaxFlashlightIntensity() const { return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Flashlight->Intensity; };

	UFUNCTION(BlueprintPure, Category = Character)
	FORCEINLINE EPhysicalSurface GetCurrentSurface() const { return CurrentSurface; };
	
private:
	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION()
	void OnRep_IsFlashlightOn();
	
	UFUNCTION()
	void OnRep_HandheldInventory(TArray<AHandheld*> HandheldInventoryBefore);

	UFUNCTION()
	void OnRep_EquippedHandheld(AHandheld* LastEquippedHandheld);

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION()
	void OnRep_IsDying();

};
