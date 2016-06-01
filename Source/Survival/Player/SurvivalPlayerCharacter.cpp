// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerCharacter.h"
#include "Survival/Game/SurvivalGameMode.h"
#include "SurvivalPlayerState.h"
#include "Survival/Pickups/Pickup.h"
#include "Survival/Equipment/Weapons/Weapon.h"

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
	Camera->AttachTo(GetCapsuleComponent());
	Camera->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	Camera->bUsePawnControlRotation = true;

	// Create an extra MeshComponent for the first person mesh
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->AttachTo(Camera);
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->CastShadow = false;

	// Set 3rd person mesh invisible for owner
	GetMesh()->SetOwnerNoSee(true);

	//Create SpotLightComponent for the flashlight
	Flashlight = CreateDefaultSubobject<USpotLightComponent>("Flashlight");
	Flashlight->AttachTo(Camera);
	Flashlight->OuterConeAngle = 28.0f;
	Flashlight->InnerConeAngle = 20.0f;
	Flashlight->Intensity = 1.5f;
	Flashlight->bUseInverseSquaredFalloff = false;
	Flashlight->AttenuationRadius = 2500.0f;
	Flashlight->bAffectDynamicIndirectLighting = true;
	Flashlight->bAffectTranslucentLighting = true;
	Flashlight->bUseRayTracedDistanceFieldShadows = true;

	bIsFlashlightOn = false;
	Flashlight->SetVisibility(bIsFlashlightOn);

	SurvivalCharacterMovement = Cast<USurvivalCharacterMovement>(Super::GetCharacterMovement());

	bIsDying = false;
	bIsDead = false;
	Health = 100.0f;

	DamagingLandingVelocity = 1000.0f;
	DeadlyLandingVelocity = 1500.0f;

	bIsSprinting = false;

	PickupRange = 250.0f;
	
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

	// Spawn the first handheld from the loadout list
	if (HasAuthority() && StartEquipment.Num() > 0)
	{
		SpawnHandheld(StartEquipment[0]);
	}

	if (HasAuthority())
	{
		for (FAmmo& Ammo : StartAmmunition)
		{
			AmmunitionInventory.AddAmmo(Ammo.Type, Ammo.Amount);
		}
	}

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

	InputComponent->BindAction("Flashlight", IE_Pressed, this, &ASurvivalPlayerCharacter::ToggleFlashlight);

	InputComponent->BindAction("Pickup", IE_Pressed, this, &ASurvivalPlayerCharacter::Pickup);
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

}

void ASurvivalPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		Flashlight->SetWorldRotation(GetBaseAimRotation());
	}

	if (IsLocallyControlled())
	{
		UpdateTargetPickup();
	}
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
	if (Health <= 0.0f)
	{
		return 0.0f;
	}
	
	Damage = FMath::Max(0.0f, Damage);

	const float TakenDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Max(0.0f, Health - TakenDamage);

	if (Health <= 0.0f)
	{
		Die(DamageEvent, EventInstigator, false);
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
		bIsDying = true;
		
		ASurvivalPlayerController* SurvivalPlayerController = GetPlayerController();
		float DieDelay = SurvivalPlayerController ? SurvivalPlayerController->GetMinDieDelay() : 0.0f;
		if (DieDelay >= 0.1f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Die, this, &ASurvivalPlayerCharacter::Die, DieDelay);

			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();

			GetMesh1P()->SetVisibility(false, true);
		}
		else
		{
			Die();
		}
	}
}

void ASurvivalPlayerCharacter::Die()
{
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

	if (EquippedHandheld)
	{
		EquippedHandheld->Destroy();
	}

	DetachFromControllerPendingDestroy();
}

void ASurvivalPlayerCharacter::Revive()
{
	if (bIsDying && !bIsDead)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Die);
		SetLifeSpan(0.0f);

		bIsDying = false;

		GetMesh1P()->SetVisibility(true, true);
	}
}

float ASurvivalPlayerCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Health;
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

void ASurvivalPlayerCharacter::AddAmmo(TSubclassOf<AWeaponProjectile> Type, int32 Amount)
{
	AmmunitionInventory.AddAmmo(Type, Amount);
}

int32 ASurvivalPlayerCharacter::RequestAmmo(TSubclassOf<AWeaponProjectile> Type, int32 Amount)
{
	return AmmunitionInventory.RequestAmmo(Type, Amount);
}

int32 ASurvivalPlayerCharacter::GetAmmoAmmountOfType(TSubclassOf<AWeaponProjectile> Type) const
{
	return AmmunitionInventory.GetAmmoAmmountOfType(Type);
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
	SetSprint(true);
}

void ASurvivalPlayerCharacter::StopSprint()
{
	SetSprint(false);
}

void ASurvivalPlayerCharacter::SetSprint(bool bShouldSprint)
{
	if (bShouldSprint)
	{
		StopCrouch();
	}

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
	if (bIsSprinting)
	{
		StopSprint();
	}

	Crouch();
}

void ASurvivalPlayerCharacter::StopCrouch()
{
	UnCrouch();
}

void ASurvivalPlayerCharacter::ToggleFlashlight()
{
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
	Flashlight->SetVisibility(bOn);
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

void ASurvivalPlayerCharacter::SpawnHandheld_Implementation(TSubclassOf<AHandheld> HandheldClass)
{
	if (HandheldClass == nullptr)
		return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.Owner = this;

	AHandheld* NewHandheld = GetWorld()->SpawnActor<AHandheld>(HandheldClass, SpawnParameters);
	NewHandheld->SetOwnerCharacter(this);

	Equip(NewHandheld);
}

void ASurvivalPlayerCharacter::Equip(AHandheld* Handheld)
{
	if (HasAuthority())
	{
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
	if (EquippedHandheld != nullptr)
	{
		EquippedHandheld->UnEquip();
	}

	if (Handheld != nullptr)
	{
		Handheld->SetOwnerCharacter(this);
		Handheld->Equip();
	}

	EquippedHandheld = Handheld;
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

void ASurvivalPlayerCharacter::UpdateTargetPickup()
{
	APickup* NewTargetPickup;

	FHitResult HitResult;

	FVector TraceStart = GetCamera()->GetComponentLocation();
	FVector TraceEnd = TraceStart + GetBaseAimRotation().Vector() * PickupRange;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(EquippedHandheld);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, CollisionQueryParams))
	{
		NewTargetPickup = Cast<APickup>(HitResult.GetActor());
	}
	else
	{
		NewTargetPickup = nullptr;
	}

	if (TargetingPickup != NewTargetPickup)
	{
		if (TargetingPickup && TargetingPickup->GetPickupMesh())
		{
			TargetingPickup->GetPickupMesh()->SetRenderCustomDepth(false);
		}

		if (NewTargetPickup && NewTargetPickup->GetPickupMesh())
		{
			NewTargetPickup->GetPickupMesh()->SetRenderCustomDepth(true);
		}
	}

	TargetingPickup = NewTargetPickup;
}

void ASurvivalPlayerCharacter::Pickup()
{
	if (TargetingPickup == nullptr)
		return;

	ServerPickup(TargetingPickup);
}

void ASurvivalPlayerCharacter::ServerPickup_Implementation(APickup* Pickup)
{
	if (Pickup == nullptr || Pickup->IsPendingKill())
		return;

	Pickup->Pickup(this);
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

void ASurvivalPlayerCharacter::OnRep_EquippedHandheld()
{
	SimulateEquip(EquippedHandheld);
}

void ASurvivalPlayerCharacter::OnRep_IsDead()
{
	if (bIsDead)
	{
		SetRagdollPhysics();
	}
}

void ASurvivalPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsFlashlightOn, COND_SkipOwner);
	DOREPLIFETIME(ASurvivalPlayerCharacter, EquippedHandheld);
	DOREPLIFETIME(ASurvivalPlayerCharacter, Health);
	DOREPLIFETIME(ASurvivalPlayerCharacter, bIsDead);
	DOREPLIFETIME(ASurvivalPlayerCharacter, AmmunitionInventory);
}
