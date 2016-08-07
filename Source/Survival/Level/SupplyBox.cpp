// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SupplyBox.h"

ASupplyBox::ASupplyBox()
{
	CaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("CaseMesh");
	//CaseMesh->BodyInstance.bLockXRotation = true;
	//CaseMesh->BodyInstance.bLockYRotation = true;
	//CaseMesh->BodyInstance.CreateDOFLock();
	CaseMesh->SetCollisionProfileName("SupplyBox");
	RootComponent = CaseMesh;

	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>("CoverMesh");
	CoverMesh->SetCollisionProfileName("SupplyBox");
	CoverMesh->SetupAttachment(RootComponent);

	SimulatingPhysicsProfileName = "SupplyBoxPhysic";

	bIsOpen = false;

	OpenTime = 0.0f;

	bReplicates = true;
}

void ASupplyBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsOpen && CoverOpenRotationCurve != nullptr)
	{
		OpenTime += DeltaSeconds;
		CoverMesh->SetRelativeRotation(FRotator(CoverOpenRotationCurve->GetFloatValue(OpenTime), 0, 0));
	}
}

bool ASupplyBox::IsInteractable_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	return !bIsOpen && GetVelocity().Size() < 1.0f;
}

bool ASupplyBox::IsRenderedAsInteractable_Implementation()
{
	return !bIsOpen;
}

void ASupplyBox::Interact_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	if (bIsOpen)
		return;

	Open();
}

void ASupplyBox::Open()
{
	bIsOpen = true;
}

void ASupplyBox::OnRep_IsOpen()
{
	if (bIsOpen)
	{
		Open();
	}
}

void ASupplyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASupplyBox, bIsOpen);
}
