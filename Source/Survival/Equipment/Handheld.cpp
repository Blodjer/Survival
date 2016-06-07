// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Handheld.h"


AHandheld::AHandheld()
{
	// Create the first person mesh
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetCastShadow(false);
	RootComponent = Mesh1P;

	// Create the third person mesh
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->AttachTo(Mesh1P);

	bIsEquipped = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AHandheld::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UnEquip();
}

void AHandheld::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Try to get the input from the owner character. Setup input actions when ready
	if (OwnerInputComponent == nullptr)
	{
		if (OwnerCharacter && OwnerCharacter->InputComponent != nullptr)
		{
			OwnerInputComponent = OwnerCharacter->InputComponent;

			if (bIsEquipped)
			{
				SetupInputActions();
			}
		}
	}
}

void AHandheld::Destroyed()
{
	ClearActionBindings();

	Super::Destroyed();
}

void AHandheld::SetOwnerCharacter(ASurvivalPlayerCharacter* Character)
{
	OwnerCharacter = Character;
	OwnerInputComponent = Character ? Character->InputComponent : nullptr;
}

void AHandheld::Equip()
{
	if (OwnerCharacter == nullptr)
		return;

	// Attach meshes to owner meshes
	FName AttachPoint = OwnerCharacter->GetHandheldAttachPoint();
	Mesh1P->AttachTo(OwnerCharacter->GetMesh1P(), AttachPoint);
	Mesh3P->AttachTo(OwnerCharacter->GetMesh(), AttachPoint);

	Mesh1P->SetVisibility(true);
	Mesh3P->SetVisibility(true);

	// TODO: Animation
	
	SetupInputActions();

	bIsEquipped = true;
}

void AHandheld::UnEquip()
{
	ClearActionBindings();

	//Mesh1P->DetachFromParent();
	Mesh1P->SetVisibility(false);

	//Mesh3P->DetachFromParent();
	Mesh3P->SetVisibility(false);

	bIsEquipped = false;
}

bool AHandheld::IsEquipped() const
{
	return bIsEquipped;
}

void AHandheld::ThrowAway()
{
	ClearActionBindings();

	bIsEquipped = false;

	if (PickupClass != nullptr)
	{
		if (HasAuthority() && GetWorld())
		{
			APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, GetActorLocation(), GetActorRotation());
			if (Pickup)
			{
				Pickup->SetReplicateMovement(true);
				Pickup->GetPickupMesh()->SetSimulatePhysics(true);
			}
		}

		Destroy(true);
	}
	else
	{
		if (OwnerCharacter == nullptr || !OwnerCharacter->IsLocallyControlled())
		{
			Mesh1P->SetWorldLocation(Mesh3P->GetComponentLocation());
		}

		Mesh3P->AttachTo(RootComponent, NAME_None, EAttachLocation::SnapToTarget);
		DetachRootComponentFromParent(true);

		Mesh1P->SetSimulatePhysics(true);
		Mesh1P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh1P->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SetOwner(nullptr);
		SetOwnerCharacter(nullptr);

		SetLifeSpan(5.0f);
	}
}

void AHandheld::ClearActionBindings()
{
	if (InputActionBindings.Num() == 0 || OwnerInputComponent == nullptr)
		return;

	// Iterate through each input action binded from this handheld and remove it from the owner input component
	for (FInputActionBinding InputActionBinding : InputActionBindings)
	{
		for (int32 i = 0; i < OwnerInputComponent->GetNumActionBindings(); i++)
		{
			if (OwnerInputComponent->GetActionBinding(i).ActionName == InputActionBinding.ActionName)
			{
				OwnerInputComponent->RemoveActionBinding(i);
				break;
			}
		}
	}

	InputActionBindings.Empty();
}

void AHandheld::OnRep_OwnerCharacter()
{
	SetOwnerCharacter(OwnerCharacter);

	if (OwnerCharacter == nullptr)
	{
		ThrowAway();
	}
}

void AHandheld::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHandheld, OwnerCharacter);
}
