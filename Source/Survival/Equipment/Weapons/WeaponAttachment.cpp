// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "WeaponAttachment.h"

AWeaponAttachment::AWeaponAttachment()
{
	Mesh1P = CreateDefaultSubobject<UStaticMeshComponent>("Mesh1P");
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetCastShadow(false);
	Mesh1P->SetCollisionProfileName("NoCollision");
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh1P;

	Mesh3P = CreateDefaultSubobject<UStaticMeshComponent>("Mesh3P");
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetCollisionProfileName("NoCollision");
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetupAttachment(Mesh1P);

	PrimaryActorTick.bCanEverTick = false;
}
