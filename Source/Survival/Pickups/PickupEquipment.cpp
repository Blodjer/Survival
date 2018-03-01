// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "PickupEquipment.h"
#include "Survival/Equipment/Weapons/Weapon.h"
#include "Survival/Equipment/Weapons/WeaponAttachment.h"

void APickupEquipment::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HandheldClass != nullptr)
	{
		AWeapon* Weapon = Cast<AWeapon>(HandheldClass.GetDefaultObject());
		if (Weapon && Weapon->GetDefaultSight())
		{
			AWeaponAttachment* WeaponSight = Weapon->GetDefaultSight().GetDefaultObject();
			
			UStaticMeshComponent* SightMesh = NewObject<UStaticMeshComponent>(this, "SightMesh");
			SightMesh->SetStaticMesh(WeaponSight->GetMesh3P()->GetStaticMesh());
			SightMesh->SetupAttachment(RootComponent);
			SightMesh->RegisterComponent();
		}
	}
}

void APickupEquipment::OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter)
{
	if (HandheldClass != nullptr)
	{
		PlayerCharacter->AddHandheldToInventory(HandheldClass);
	}

	Super::OnPickup_Implementation(PlayerCharacter);
}
