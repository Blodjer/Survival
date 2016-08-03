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
	Mesh3P->SetupAttachment(Mesh1P);

	Type = EHandheldType::Unknown;

	bIsEquipped = false;
	bIsWaste = false;

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

	// TODO: Animation
	
	SetupInputActions();

	bIsEquipped = true;
}

void AHandheld::UnEquip()
{
	ClearActionBindings();

	OnCharacterStopUse();

	bIsEquipped = false;
}

void AHandheld::Drop(bool bIsWaste)
{
	if (GetOwnerCharacter() == nullptr)
		return;

	BeforeDrop();

	ClearActionBindings();

	GetOwnerCharacter()->RemoveHandheldFromInventory(this);

	bIsEquipped = false;
	this->bIsWaste = bIsWaste;
	if (!bIsWaste && PickupClass != nullptr)
	{
		if (HasAuthority() && GetWorld())
		{
			FTransform PickupTransform(GetActorRotation(), GetActorLocation());
			APickup* Pickup = GetWorld()->SpawnActorDeferred<APickup>(PickupClass, PickupTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (Pickup)
			{
				FVector DropVelocity = FVector::ZeroVector;
				if (GetOwnerCharacter())
				{
					DropVelocity = GetOwnerCharacter()->GetVelocity() + GetOwnerCharacter()->GetControlRotation().Vector() * 200.0f;
					DropVelocity += FVector(0, 0, 80.0f);
				}
				Pickup->StartSimulatePhysics(DropVelocity);
				UGameplayStatics::FinishSpawningActor(Pickup, PickupTransform);
			}
		}

		Destroy(true);
	}
	else
	{
		if (!OwnerCharacter->IsLocallyControlled())
		{
			Mesh1P->SetWorldLocation(Mesh3P->GetComponentLocation());
		}

		Mesh3P->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		DetachRootComponentFromParent();

		if (OwnerCharacter->IsLocallyControlled())
			SetActorLocation(OwnerCharacter->GetMesh1P()->GetSocketLocation(OwnerCharacter->GetHandheldAttachPoint()));
		else
			SetActorLocation(OwnerCharacter->GetMesh()->GetSocketLocation(OwnerCharacter->GetHandheldAttachPoint()));

		Mesh1P->SetVisibility(true);
		Mesh3P->SetVisibility(true);

		Mesh1P->SetSimulatePhysics(true);
		Mesh1P->SetCollisionProfileName("PickupPhysic");
		Mesh1P->SetAllPhysicsLinearVelocity(GetOwnerCharacter() ? GetOwnerCharacter()->GetVelocity() : FVector::ZeroVector);
		
		SetLifeSpan(10.0f);

		TearOff();
	}
}

bool AHandheld::IsGameInputAllowed() const
{
	return GetOwnerCharacter() ? GetOwnerCharacter()->IsGameInputAllowed() : false;
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
}

void AHandheld::OnRep_IsWasted()
{
	if (bIsWaste)
	{
		Drop(true);
	}
}

void AHandheld::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHandheld, OwnerCharacter);
	DOREPLIFETIME(AHandheld, bIsWaste);
}
