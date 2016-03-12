// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerCharacter.h"


ASurvivalPlayerCharacter::ASurvivalPlayerCharacter()
{
	// Set base input rates for controllers
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

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
	Flashlight->SetVisibility(false);

	bIsFlashlightOn = false;

	PrimaryActorTick.bCanEverTick = true;
}

void ASurvivalPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASurvivalPlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!IsLocallyControlled())
	{
		Flashlight->SetWorldRotation(GetBaseAimRotation());
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

	// Mouse
	InputComponent->BindAxis("Turn", this, &ASurvivalPlayerCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &ASurvivalPlayerCharacter::LookUp);

	// Controller
	InputComponent->BindAxis("TurnAtRate", this, &ASurvivalPlayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ASurvivalPlayerCharacter::LookUpAtRate);

	InputComponent->BindAction("Flashlight", IE_Pressed, this, &ASurvivalPlayerCharacter::ToggleFlashlight);
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

void ASurvivalPlayerCharacter::OnRep_IsFlashlightOn()
{
	SetFlashlightOn(bIsFlashlightOn);
}

void ASurvivalPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASurvivalPlayerCharacter, bIsFlashlightOn, COND_SkipOwner);
}
