// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerCharacter.h"
#include "Survival/Game/SurvivalGameMode.h"
#include "SurvivalPlayerState.h"

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

	Health = 100.0f;

	bIsSprinting = false;
	
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
}

void ASurvivalPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UpdateTeamColors();
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
}

void ASurvivalPlayerCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);

	Die(FDamageEvent(dmgType.GetClass()), nullptr);
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
		Die(DamageEvent, EventInstigator);
	}

	return TakenDamage;
}

void ASurvivalPlayerCharacter::Die(const FDamageEvent& DamageEvent, AController* Killer)
{
	Health = 0.0f;

	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		ASurvivalGameMode* SurvialGameMode = Cast<ASurvivalGameMode>(GetWorld()->GetAuthGameMode());
		const UDamageType* DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		SurvialGameMode->Killed(DamageType, Killer, this->GetController());
	}

	DetachFromControllerPendingDestroy();

	SetLifeSpan(1.0f);
}

float ASurvivalPlayerCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASurvivalPlayerCharacter>()->Health;
}

FTeamInfo ASurvivalPlayerCharacter::GetTeamInfo()
{
	if (PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(PlayerState);
		if (SurvivalPlayerState)
		{
			return SurvivalPlayerState->GetTeamInfo();
		}
	}
	
	return FTeamInfo();
}

int32 ASurvivalPlayerCharacter::GetTeamNumber()
{
	if (PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(PlayerState);
		if (SurvivalPlayerState)
		{
			return SurvivalPlayerState->GetTeamNumber();
		}
	}

	return -1;
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
				MID->SetVectorParameterValue("TeamColor", SurvivalPlayerState->GetTeamInfo().Color);
			}
		}
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

void ASurvivalPlayerCharacter::OnRep_EquippedHandheld()
{
	SimulateEquip(EquippedHandheld);
}

void ASurvivalPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsFlashlightOn, COND_SkipOwner);
	DOREPLIFETIME(ASurvivalPlayerCharacter, EquippedHandheld);
	DOREPLIFETIME(ASurvivalPlayerCharacter, Health);
}
