// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerCharacter.h"
#include "Survival/Game/SurvivalGameMode.h"
#include "SurvivalPlayerState.h"
#include "Survival/Pickups/Pickup.h"
#include "Pickups/PickupEquipment.h"
#include "Survival/Equipment/Weapons/Weapon.h"
#include "Survival/Level/Campfire.h"

ASurvivalPlayerCharacter::ASurvivalPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USurvivalCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	// Set base input rates for controllers
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// Set base eye height for camera
	BaseEyeHeight = 80.0f;

	// Create CameraComponent for the first person camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	Camera->bUsePawnControlRotation = true;

	// Create an extra MeshComponent for the first person mesh
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(Camera);
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->CastShadow = false;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	// Set 3rd person mesh invisible for owner
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	//Create SpotLightComponent for the flashlight
	Flashlight = CreateDefaultSubobject<USpotLightComponent>("Flashlight");
	Flashlight->SetupAttachment(Camera);
	Flashlight->OuterConeAngle = 28.0f;
	Flashlight->InnerConeAngle = 20.0f;
	Flashlight->Intensity = 1.5f;
	Flashlight->bUseInverseSquaredFalloff = false;
	Flashlight->AttenuationRadius = 2500.0f;
	Flashlight->bAffectDynamicIndirectLighting = true;
	Flashlight->bAffectTranslucentLighting = true;
	Flashlight->bUseRayTracedDistanceFieldShadows = true;

	bIsFlashlightOn = false;
	Flashlight->SetVisibility(bIsFlashlightOn, true);
	FlashlightBatteryPowerDrain = 0.016f;

	SurvivalCharacterMovement = Cast<USurvivalCharacterMovement>(Super::GetCharacterMovement());

	bIsLethalInjured = false;
	bIsDead = false;
	Health = 100.0f;

	DamagingLandingVelocity = 1000.0f;
	DeadlyLandingVelocity = 1500.0f;

	StartBatteryPower = 1.0f;

	bIsSprinting = false;

	Stamina = 2.0f;
	StaminaDecrease = 0.1f;

	InteractionRange = 250.0f;

	UpdateSurfaceRate = 0.2f;
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASurvivalPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set the camera height to eye height
	Camera->RelativeLocation = FVector(0, 0, BaseEyeHeight);
}

void ASurvivalPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		for (FAmmo& Ammo : StartAmmunition)
		{
			AmmunitionInventory.AddAmmo(Ammo.Type, Ammo.Amount);
		}
	}

	// Spawn the first handheld from the loadout list
	if (HasAuthority() && StartEquipment.Num() > 0)
	{
		for (TSubclassOf<AHandheld> HandheldClass : StartEquipment)
		{
			AddHandheldToInventory(HandheldClass);
		}
	}

	CurrentBatteryPower = StartBatteryPower;

	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(i));
	}
	for (int32 i = 0; i < GetMesh1P()->GetNumMaterials(); i++)
	{
		MeshMIDs.Add(GetMesh1P()->CreateAndSetMaterialInstanceDynamic(i));
	}
}

void ASurvivalPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	if (InputComponent == nullptr)
		return;
	
	InputComponent->BindAction("Jump", IE_Pressed, this, &ASurvivalPlayerCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASurvivalPlayerCharacter::StopJumping);
	
	InputComponent->BindAxis("MoveForward", this, &ASurvivalPlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASurvivalPlayerCharacter::MoveRight);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &ASurvivalPlayerCharacter::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ASurvivalPlayerCharacter::StopSprint);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ASurvivalPlayerCharacter::StartCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &ASurvivalPlayerCharacter::StopCrouch);

	InputComponent->BindAction("CrouchToggle", IE_Pressed, this, &ASurvivalPlayerCharacter::ToggleCrouch);

	// Mouse
	InputComponent->BindAxis("Turn", this, &ASurvivalPlayerCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &ASurvivalPlayerCharacter::LookUp);

	// Controller
	InputComponent->BindAxis("TurnAtRate", this, &ASurvivalPlayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ASurvivalPlayerCharacter::LookUpAtRate);

	InputComponent->BindAction("NextHandheld", IE_Pressed, this, &ASurvivalPlayerCharacter::NextHandheld);
	InputComponent->BindAction("PreviousHandheld", IE_Pressed, this, &ASurvivalPlayerCharacter::PreviousHandheld);

	InputComponent->BindAction("EquipSlot_1", IE_Pressed, this, &ASurvivalPlayerCharacter::EquipSlot<0>);
	InputComponent->BindAction("EquipSlot_2", IE_Pressed, this, &ASurvivalPlayerCharacter::EquipSlot<1>);
	InputComponent->BindAction("EquipSlot_3", IE_Pressed, this, &ASurvivalPlayerCharacter::EquipSlot<2>);

	InputComponent->BindAction("DropHandheld", IE_Pressed, this, &ASurvivalPlayerCharacter::DropHandheld);

	InputComponent->BindAction("Flashlight", IE_Pressed, this, &ASurvivalPlayerCharacter::ToggleFlashlight);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalPlayerCharacter::Interact);
}

void ASurvivalPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsLocallyControlled() && IsPlayerControlled())
	{
		OnPossessedLocal();
	}

	UpdateTeamColors();
}

void ASurvivalPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsLocallyControlled())
	{
		OnPossessedLocal();
	}
}

void ASurvivalPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UpdateTeamColors();
}

void ASurvivalPlayerCharacter::OnUpdateTeamInfo()
{
	UpdateTeamColors();
}

void ASurvivalPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_UpdateSurface, this, &ASurvivalPlayerCharacter::UpdateSurface, UpdateSurfaceRate, true);
}

void ASurvivalPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsSprinting())
	{
		if (bIsCrouched)
		{
			StopCrouch();
		}

		if (EquippedHandheld != nullptr)
		{
			EquippedHandheld->OnCharacterStopUse();
		}
	}

	if (!IsLocallyControlled())
	{
		Flashlight->SetWorldRotation(GetBaseAimRotation());
	}

	if (bIsFlashlightOn)
	{
		DrainBatteryPower(FlashlightBatteryPowerDrain * DeltaTime);
	}
	Flashlight->SetIntensity(GetMaxFlashlightIntensity() * FMath::Pow(CurrentBatteryPower, 2.0f));

	if (IsLocallyControlled())
	{
		UpdateTargetInteractable();
	}

	if (IsSprinting() && GetCharacterMovement() && GetVelocity().Size() >= GetCharacterMovement()->MaxWalkSpeed)
	{
		Stamina = FMath::Max(0.0f, Stamina - StaminaDecrease * DeltaTime);
	}
	else
	{
		Stamina = FMath::Min(GetMaxStamina(), Stamina + StaminaIncrease.GetValue(this) * DeltaTime);
	}
}

void ASurvivalPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyInventory();

	Super::EndPlay(EndPlayReason);
}

void ASurvivalPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (FMath::Abs(GetVelocity().Z) > DamagingLandingVelocity)
	{
		float Damage = (FMath::Abs(GetVelocity().Z) - DamagingLandingVelocity) / (DeadlyLandingVelocity - DamagingLandingVelocity) * GetMaxHealth();
		TakeDamage(Damage, FDamageEvent(), nullptr, Hit.GetActor());
	}
}

void ASurvivalPlayerCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);

	Die(FDamageEvent(dmgType.GetClass()), nullptr, true);
}

float ASurvivalPlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
		return 0.0f;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		FName HitBoneName = PointDamageEvent->HitInfo.BoneName;

		float DamageMultiplier = 1.0f;
		for (FHitZone& HitZone : HitZones)
		{
			if (GetMesh()->BoneIsChildOf(HitBoneName, HitZone.ParentBoneName) || HitBoneName == HitZone.ParentBoneName)
			{
				DamageMultiplier = HitZone.DamageMultiplier;
			}
		}
		Damage *= DamageMultiplier;

		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, HitBoneName.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::FromInt(GetMesh()->GetBoneIndex(HitBoneName)));
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::SanitizeFloat(DamageMultiplier));
	}
	
	Damage = FMath::Max(0.0f, Damage);

	const float TakenDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (EventInstigator && EventInstigator->GetPawn())
	{
		ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(EventInstigator->GetPawn());
		if (SurvivalPlayerCharacter)
		{
			SurvivalPlayerCharacter->ConfirmDamage(TakenDamage, DamageEvent);
		}
	}

	if (TakenDamage > 0.0f && Health > 0.0f)
	{
		Health = FMath::Max(0.0f, Health - TakenDamage);

		if (Health <= 0.0f)
		{
			Die(DamageEvent, EventInstigator, false);
		}
	}

	return TakenDamage;
}

void ASurvivalPlayerCharacter::Die(const FDamageEvent& DamageEvent, AController* Killer, bool bImmediately)
{
	Health = 0.0f;

	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		ASurvivalGameMode* SurvialGameMode = Cast<ASurvivalGameMode>(GetWorld()->GetAuthGameMode());
		const UDamageType* DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		SurvialGameMode->Killed(DamageType, Killer, this->GetController());
	}

	if (bImmediately)
	{
		Die();
	}
	else
	{
		InjureLethal();
	}
}

void ASurvivalPlayerCharacter::InjureLethal()
{
	bIsLethalInjured = true;

	DisableInput(GetPlayerController());

	if (HasAuthority() && EquippedHandheld != nullptr)
	{
		PreviousEquippedHandheld = EquippedHandheld;
		EquippedHandheld->UnEquip();
		EquippedHandheld = nullptr;
	}

	if (CapturingCampfire != nullptr)
	{
		CapturingCampfire->UpdateCapturingPlayers();
	}

	ASurvivalPlayerController* SurvivalPlayerController = GetPlayerController();
	float DieDelay = SurvivalPlayerController ? SurvivalPlayerController->GetMinDieDelay() : 0.0f;
	if (DieDelay >= 0.1f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Die, this, &ASurvivalPlayerCharacter::Die, DieDelay);
	}
	else
	{
		Die();
	}
}

void ASurvivalPlayerCharacter::Die()
{
	if (!HasAuthority())
		return;

	bIsDead = true;

	SetReplicateMovement(false);

	if (GetController())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		float RespawnDelay = PlayerController ? PlayerController->GetMinRespawnDelay() : 0.0f;
		if (RespawnDelay > 0.0f)
		{
			SetLifeSpan(RespawnDelay);
			SetRagdollPhysics();
		}
		else
		{
			Destroy();
		}
	}
	else
	{
		Destroy();
	}

	DetachFromControllerPendingDestroy();
}

void ASurvivalPlayerCharacter::ConfirmDamage_Implementation(float Damage, FDamageEvent const& DamageEvent)
{
	OnConfirmDamage.Broadcast(Damage, DamageEvent);
}

void ASurvivalPlayerCharacter::Revive(float NewHealth)
{
	if (!bIsDead)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Die);

		bIsLethalInjured = false;
		Health = FMath::Clamp(NewHealth, 1.0f, GetMaxHealth());

		if (PreviousEquippedHandheld != nullptr)
		{
			Equip(PreviousEquippedHandheld);
		}

		if (CapturingCampfire != nullptr)
		{
			CapturingCampfire->UpdateCapturingPlayers();
		}

		EnableInput(GetPlayerController());
	}
}

void ASurvivalPlayerCharacter::Heal(float Value)
{
	Health = FMath::Min(Health + FMath::Max(0.0f, Value), GetMaxHealth());
}

bool ASurvivalPlayerCharacter::GetTeamInfo(FTeamInfo& TeamInfo)
{
	if (PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(PlayerState);
		if (SurvivalPlayerState)
		{
			return SurvivalPlayerState->GetTeamInfo(TeamInfo);
		}
	}
	
	return false;
}

int32 ASurvivalPlayerCharacter::GetTeamIdx()
{
	if (PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(PlayerState);
		if (SurvivalPlayerState)
		{
			return SurvivalPlayerState->GetTeamIdx();
		}
	}

	return -1;
}

AWeapon* ASurvivalPlayerCharacter::GetEquippedWeapon() const
{
	return EquippedHandheld != nullptr ? Cast<AWeapon>(EquippedHandheld) : nullptr;
}

int32 ASurvivalPlayerCharacter::AddAmmo(TSubclassOf<AWeaponProjectile> Type, int32 Amount, bool bOverfill)
{
	return AmmunitionInventory.AddAmmo(Type, Amount, bOverfill);
}

int32 ASurvivalPlayerCharacter::RequestAmmo(TSubclassOf<AWeaponProjectile> Type, int32 Amount)
{
	return AmmunitionInventory.RequestAmmo(Type, Amount);
}

int32 ASurvivalPlayerCharacter::GetAmmoAmountOfType(TSubclassOf<AWeaponProjectile> Type) const
{
	return AmmunitionInventory.GetAmmoAmountOfType(Type);
}

int32 ASurvivalPlayerCharacter::GetRemainingAmmoAmountOfType(TSubclassOf<AWeaponProjectile> Type) const
{
	return AmmunitionInventory.GetRemainingAmmoAmountOfType(Type);
}

const TScriptInterface<IInteractable> ASurvivalPlayerCharacter::GetInteractableInterface(AActor* Actor) const
{
	IInteractable* TargetingInterface = Cast<IInteractable>(Actor);
	TScriptInterface<IInteractable> TargetingScriptInterface = TScriptInterface<IInteractable>();
	TargetingScriptInterface.SetObject(Actor);
	TargetingScriptInterface.SetInterface(TargetingInterface);

	return TargetingScriptInterface;
}

const TScriptInterface<IInteractable> ASurvivalPlayerCharacter::GetTargetingInteractableInterface() const
{
	return GetInteractableInterface(TargetingInteractableActor);
}

bool ASurvivalPlayerCharacter::CanPickup(APickup* Pickup)
{
	if (Pickup == nullptr)
		return false;
	
	if (Pickup->IsA(APickupEquipment::StaticClass()))
	{
		APickupEquipment* PickupEquipment = Cast<APickupEquipment>(Pickup);
		EHandheldType PickupHandheldType = PickupEquipment->HandheldClass.GetDefaultObject()->GetHandheldType();
		
		int AvailableSlots = HandheldInventorySlots.GetAvailableSlots(PickupHandheldType);
		if (EquippedHandheld && EquippedHandheld->GetHandheldType() == PickupHandheldType)
		{
			AvailableSlots--;
		}
		return AvailableSlots > 0;
	}

	return true;
}

void ASurvivalPlayerCharacter::AddBatteryPower(float Amount)
{
	if (Amount <= 0.0f)
		return;

	CurrentBatteryPower = FMath::Min(CurrentBatteryPower + Amount, 1.0f);
}

void ASurvivalPlayerCharacter::DrainBatteryPower(float Amount)
{
	CurrentBatteryPower = FMath::Max(0.0f, CurrentBatteryPower - FMath::Abs(Amount));
}

float ASurvivalPlayerCharacter::GetDeadness() const
{
	ASurvivalPlayerController* SurvivalPlayerController = GetPlayerController();
	float DieDelay = SurvivalPlayerController ? SurvivalPlayerController->GetMinDieDelay() : 0.0f;
	return GetWorldTimerManager().GetTimerElapsed(TimerHandle_Die) / DieDelay;
}

bool ASurvivalPlayerCharacter::IsGameInputAllowed() const
{
	ASurvivalPlayerController* SurvivalPlayerController = GetPlayerController();
	return SurvivalPlayerController ? SurvivalPlayerController->IsGameInputAllowed() : true;
}

void ASurvivalPlayerCharacter::OnOpenPauseMenu()
{
	if (GetEquippedHandheld())
	{
		GetEquippedHandheld()->OnCharacterStopUse();
	}
}

void ASurvivalPlayerCharacter::SetRagdollPhysics()
{
	if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
		return;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
}

void ASurvivalPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASurvivalPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASurvivalPlayerCharacter::Turn(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value);
	}
}

void ASurvivalPlayerCharacter::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerPitchInput(Value);
	}
}

void ASurvivalPlayerCharacter::TurnAtRate(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ASurvivalPlayerCharacter::LookUpAtRate(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ASurvivalPlayerCharacter::StartSprint()
{
	if (IsMoveInputIgnored())
		return;
	
	SetSprint(true);
}

void ASurvivalPlayerCharacter::StopSprint()
{
	SetSprint(false);
}

void ASurvivalPlayerCharacter::SetSprint(bool bShouldSprint)
{
	if (SurvivalCharacterMovement)
	{
		SurvivalCharacterMovement->bWantsToSprint = bShouldSprint;
		bIsSprinting = bShouldSprint;
	}

	if (!HasAuthority())
	{
		ServerSetSprint(bShouldSprint);
	}
}

void ASurvivalPlayerCharacter::ServerSetSprint_Implementation(bool bShouldSprint)
{
	if (bShouldSprint)
	{
		StartSprint();
	}
	else
	{
		StopSprint();
	}
}

void ASurvivalPlayerCharacter::Jump()
{
	if (IsMoveInputIgnored())
		return;

	StopCrouch();

	Super::Jump();
}

void ASurvivalPlayerCharacter::ToggleCrouch()
{
	if (bIsCrouched)
	{
		StopCrouch();
	}
	else
	{
		StartCrouch();
	}
}

void ASurvivalPlayerCharacter::StartCrouch()
{
	if (IsMoveInputIgnored())
		return;

	if (bIsSprinting)
	{
		StopSprint();
	}

	Crouch();
}

void ASurvivalPlayerCharacter::StopCrouch()
{
	if (IsMoveInputIgnored())
		return;

	UnCrouch();
}

void ASurvivalPlayerCharacter::ToggleFlashlight()
{
	if (!IsGameInputAllowed())
		return;

	if (bIsFlashlightOn)
	{
		SetFlashlightOn(false);
	}
	else
	{
		SetFlashlightOn(true);
	}
}

void ASurvivalPlayerCharacter::SetFlashlightOn(bool bOn)
{
	Flashlight->SetVisibility(bOn, true);
	bIsFlashlightOn = bOn;

	if (!HasAuthority())
	{
		ServerSetFlashlightOn(bOn);
	}
}

void ASurvivalPlayerCharacter::ServerSetFlashlightOn_Implementation(bool bOn)
{
	SetFlashlightOn(bOn);
}

void ASurvivalPlayerCharacter::AddHandheldToInventory(TSubclassOf<AHandheld> HandheldClass)
{
	if (HandheldClass == nullptr)
		return;

	if (HandheldInventory.ContainsByPredicate([HandheldClass](AHandheld* Handheld) { return Handheld->IsA(HandheldClass); }))
		return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.Owner = this;

	AHandheld* NewHandheld = GetWorld()->SpawnActor<AHandheld>(HandheldClass, SpawnParameters);
	if (NewHandheld)
	{
		NewHandheld->SetOwnerCharacter(this);

		HandheldInventory.Add(NewHandheld);

		if (HandheldInventory.Num() == 1 && EquippedHandheld == nullptr)
		{
			Equip(NewHandheld);
		}
		else
		{
			FName InventorySocketName = HandheldInventorySlots.Store(NewHandheld);
			if (!InventorySocketName.IsNone())
			{
				NewHandheld->GetMesh1P()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, InventorySocketName);
				NewHandheld->GetMesh3P()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, InventorySocketName);

				NewHandheld->GetMesh1P()->SetVisibility(false, true);
			}
			else
			{
				NewHandheld->GetMesh1P()->SetVisibility(false, true);
				NewHandheld->GetMesh3P()->SetVisibility(false, true);
			}
		}
	}
}

void ASurvivalPlayerCharacter::RemoveHandheldFromInventory(AHandheld* Handheld)
{
	if (Handheld == nullptr)
		return;

	HandheldInventory.Remove(Handheld);
	HandheldInventorySlots.Take(Handheld);

	if (EquippedHandheld == Handheld)
	{
		EquippedHandheld = nullptr;
		PreviousHandheld();
	}
}

void ASurvivalPlayerCharacter::DestroyInventory()
{
	if (!HasAuthority())
		return;

	for (AHandheld* Handheld : HandheldInventory)
	{
		Handheld->Destroy();
	}
	HandheldInventory.Empty();

	EquippedHandheld = nullptr;
	PreviousEquippedHandheld = nullptr;
}

void ASurvivalPlayerCharacter::Equip(AHandheld* Handheld)
{
	if (HasAuthority())
	{
		SimulateUnEquip(EquippedHandheld);
		SimulateEquip(Handheld);
	}
	else
	{
		ServerEquip(Handheld);
	}
}

void ASurvivalPlayerCharacter::ServerEquip_Implementation(AHandheld* Handheld)
{
	Equip(Handheld);
}

void ASurvivalPlayerCharacter::SimulateEquip(AHandheld* Handheld)
{
	PreviousEquippedHandheld = EquippedHandheld;

	if (Handheld != nullptr)
	{
		HandheldInventorySlots.Take(Handheld);

		Handheld->GetMesh1P()->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetHandheldAttachPoint());
		Handheld->GetMesh3P()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetHandheldAttachPoint());

		Handheld->GetMesh1P()->SetVisibility(true, true);
		Handheld->GetMesh3P()->SetVisibility(true, true);

		Handheld->SetOwnerCharacter(this);
		Handheld->Equip();
	}

	EquippedHandheld = Handheld;
}

void ASurvivalPlayerCharacter::SimulateUnEquip(AHandheld* Handheld)
{
	if (Handheld != nullptr)
	{
		Handheld->UnEquip();

		if (HasAuthority())
		{
			FName InventorySocketName = HandheldInventorySlots.Store(Handheld);
			if (!InventorySocketName.IsNone())
			{
				Handheld->GetMesh1P()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, InventorySocketName);
				Handheld->GetMesh3P()->AttachToComponent(Handheld->GetMesh1P(), FAttachmentTransformRules::SnapToTargetIncludingScale, InventorySocketName);

				Handheld->GetMesh1P()->SetVisibility(false, true);
				Handheld->GetMesh3P()->SetVisibility(true, true);
			}
			else
			{
				Handheld->GetMesh1P()->SetVisibility(false, true);
				Handheld->GetMesh3P()->SetVisibility(false, true);
			}
		}
		else
		{
			if (!IsLethalInjured())
			{
				Handheld->GetMesh3P()->AttachToComponent(Handheld->GetMesh1P(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			}

			Handheld->GetMesh1P()->SetVisibility(false, true);
			Handheld->GetMesh3P()->SetVisibility(true, true);
		}
	}
}

void ASurvivalPlayerCharacter::NextHandheld()
{
	if (!IsGameInputAllowed())
		return;

	if (HandheldInventory.Num() > 1 || (EquippedHandheld == nullptr && HandheldInventory.Num() > 0))
	{
		const int32 HandheldIdx = HandheldInventory.IndexOfByKey(EquippedHandheld);
		AHandheld* NextHandheld = HandheldInventory[(HandheldIdx + 1) % HandheldInventory.Num()];
		Equip(NextHandheld);
	}
}

void ASurvivalPlayerCharacter::PreviousHandheld()
{
	if (!IsGameInputAllowed())
		return;

	if (HandheldInventory.Num() > 1 || (EquippedHandheld == nullptr && HandheldInventory.Num() > 0))
	{
		const int32 HandheldIdx = HandheldInventory.IndexOfByKey(EquippedHandheld);
		AHandheld* NextHandheld = HandheldInventory[(HandheldIdx - 1 + HandheldInventory.Num()) % HandheldInventory.Num()];
		Equip(NextHandheld);
	}
}

void ASurvivalPlayerCharacter::EquipSlot(int32 SlotIndex)
{
	if (HandheldInventorySlots.Slots.Num() > SlotIndex)
	{
		if (HandheldInventorySlots.Slots[SlotIndex].AssignedHandheld != nullptr)
		{
			Equip(HandheldInventorySlots.Slots[SlotIndex].AssignedHandheld);
		}
	}
}

void ASurvivalPlayerCharacter::UpdateTeamColors()
{
	if (PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(PlayerState);
		if (SurvivalPlayerState)
		{
			for (UMaterialInstanceDynamic* MID : MeshMIDs)
			{
				FTeamInfo TeamInfo;
				SurvivalPlayerState->GetTeamInfo(TeamInfo);
				MID->SetVectorParameterValue("TeamColor", TeamInfo.Color);
			}
		}
	}
}

void ASurvivalPlayerCharacter::UpdateTargetInteractable()
{
	AActor* NewTargetInteractable = nullptr;

	FHitResult HitResult;

	FVector TraceStart = GetCamera()->GetComponentLocation();
	FVector TraceEnd = TraceStart + GetBaseAimRotation().Vector() * InteractionRange;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(EquippedHandheld);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, CollisionQueryParams))
	{
		if (HitResult.GetActor() && HitResult.GetActor()->GetClass() && HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			if (GetInteractableInterface(HitResult.GetActor())->Execute_IsRenderedAsInteractable(HitResult.GetActor()))
			{
				NewTargetInteractable = HitResult.GetActor();
			}
		}
	}

	if (TargetingInteractableActor != NewTargetInteractable)
	{
		if (TargetingInteractableActor)
		{
			for (auto Component : TargetingInteractableActor->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
			{
				Cast<UPrimitiveComponent>(Component)->SetRenderCustomDepth(false);
			}
		}

		if (NewTargetInteractable)
		{
			for (auto Component : NewTargetInteractable->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
			{
				Cast<UPrimitiveComponent>(Component)->SetRenderCustomDepth(true);
			}
		}
	}

	TargetingInteractableActor = NewTargetInteractable;
}

void ASurvivalPlayerCharacter::Interact()
{
	if (!HasAuthority())
	{
		if (TargetingInteractableActor == nullptr || !TargetingInteractableActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			return;

		TScriptInterface<IInteractable> TargetingScriptInterface = GetTargetingInteractableInterface();

		if (TargetingScriptInterface->Execute_IsInteractable(TargetingInteractableActor, this))
		{
			ServerInteract(TargetingInteractableActor);
		}
	}
	else
	{
		ServerInteract(TargetingInteractableActor);
	}
}

void ASurvivalPlayerCharacter::ServerInteract_Implementation(AActor* Interactable)
{
	if (Interactable == nullptr || !Interactable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		return;

	TScriptInterface<IInteractable> TargetingScriptInterface = GetTargetingInteractableInterface();

	if (TargetingScriptInterface->Execute_IsInteractable(Interactable, this))
	{
		TargetingScriptInterface->Execute_Interact(Interactable, this);
	}
}

void ASurvivalPlayerCharacter::DropHandheld()
{
	if (IsGameInputAllowed())
	{
		ServerDropHandheld();
	}
}

void ASurvivalPlayerCharacter::ServerDropHandheld_Implementation()
{
	if (EquippedHandheld)
	{
		EquippedHandheld->Drop();
	}
}

void ASurvivalPlayerCharacter::UpdateSurface()
{
	FHitResult HitResult;

	FVector TraceStart = GetActorLocation() + FVector(0, 0, 20.0f);
	FVector TraceEnd = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 40.0f);

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		CurrentSurface = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	}
	else
	{
		CurrentSurface = SURFACE_Default;
	}
}

void ASurvivalPlayerCharacter::OnRep_IsSprinting()
{
	if (bIsSprinting)
	{
		StartSprint();
	}
	else
	{
		StopSprint();
	}
}

void ASurvivalPlayerCharacter::OnRep_IsFlashlightOn()
{
	SetFlashlightOn(bIsFlashlightOn);
}

void ASurvivalPlayerCharacter::OnRep_HandheldInventory(TArray<AHandheld*> HandheldInventoryBefore)
{
	TArray<AHandheld*> NewHandhelds;

	for (AHandheld* Handheld : HandheldInventory)
	{
		if (!HandheldInventoryBefore.Contains(Handheld))
		{
			NewHandhelds.Add(Handheld);
		}
	}

	for (AHandheld* Handheld : NewHandhelds)
	{
		if (!Handheld->IsEquipped())
		{
			Handheld->GetMesh1P()->SetVisibility(false, true);
			Handheld->GetMesh3P()->SetVisibility(true, true);
		}
	}
}

void ASurvivalPlayerCharacter::OnRep_EquippedHandheld(AHandheld* LastEquippedHandheld)
{
	SimulateUnEquip(LastEquippedHandheld);

	SimulateEquip(EquippedHandheld);
}

void ASurvivalPlayerCharacter::OnRep_IsDead()
{
	if (bIsDead)
	{
		SetRagdollPhysics();
	}
}

void ASurvivalPlayerCharacter::OnRep_IsDying()
{
	if (bIsLethalInjured)
	{
		InjureLethal();
	}
	else
	{
		Revive(Health);
	}
}

void ASurvivalPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsSprinting, COND_SkipOwner);

	DOREPLIFETIME(ASurvivalPlayerCharacter, EquippedHandheld);
	DOREPLIFETIME(ASurvivalPlayerCharacter, HandheldInventory);
	DOREPLIFETIME(ASurvivalPlayerCharacter, HandheldInventorySlots);
	DOREPLIFETIME(ASurvivalPlayerCharacter, AmmunitionInventory);
	DOREPLIFETIME(ASurvivalPlayerCharacter, CurrentBatteryPower);

	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsFlashlightOn, COND_SkipOwner);

	DOREPLIFETIME(ASurvivalPlayerCharacter, Health);
	DOREPLIFETIME(ASurvivalPlayerCharacter, bIsDead);
	DOREPLIFETIME(ASurvivalPlayerCharacter, bIsLethalInjured);
}
