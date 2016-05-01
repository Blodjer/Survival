// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Weapon.h"


AWeapon::AWeapon()
{
	CurrentState = EWeaponState::Idle;

	RateOfFire = 750;

	BurstCount = 0;
}

void AWeapon::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("Fire", IE_Pressed, this, &AWeapon::StartFire);
	BindInputAction("Fire", IE_Released, this, &AWeapon::StopFire);

	// TODO: Zoom

	// TODO: Reload

	// TODO: Firemode
}

void AWeapon::StartFire()
{
	if (!HasAuthority())
	{
		ServerStartFire();
	}

	if (GetOwnerCharacter()->IsLocallyControlled())
	{
		HandleFiring();
	}
}

void AWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void AWeapon::StopFire()
{
	if (!HasAuthority())
	{
		ServerStopFire();
	}

	BurstCount = 0;

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
}

void AWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

void AWeapon::HandleFiring()
{
	if (GetOwnerCharacter() != nullptr && GetOwnerCharacter()->IsLocallyControlled())
	{
		ServerShootProjectile(GetActorLocation(), GetOwnerCharacter()->GetBaseAimRotation().Vector());

		if (!GetWorld()->IsServer())
		{
			SimulateFire();
		}
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::HandleFiring, 60.0f / RateOfFire, false);
}

void AWeapon::ServerShootProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal Direction)
{
	if (ProjectileClass == nullptr || GetWorld() == nullptr)
		return;

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Instigator = Instigator;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponProjectile* NewProjectile = GetWorld()->SpawnActor<AWeaponProjectile>(ProjectileClass, Origin, Direction.Rotation(), SpawnParameters);
	if (NewProjectile)
	{
		NewProjectile->InitProjectile(Direction);

		BurstCount++;

		SimulateFire();
	}
}

void AWeapon::StartSimulateFire()
{
	SimulateFire();

	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::SimulateFire, 60.0f / RateOfFire, true);
}

void AWeapon::StopSimulateFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
}

void AWeapon::SimulateFire()
{
	if (GetOwnerCharacter() == nullptr)
		return;

	if (GetOwnerCharacter()->IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerCharacter()->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(CameraShake);
		}
	}

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetOwnerCharacter()->GetBaseAimRotation().Vector() * 100000.0f, FColor::White, false, 0.15f);
}

void AWeapon::OnRep_BurstCount()
{
	if (BurstCount > 0 && !GetWorldTimerManager().IsTimerActive(TimerHandle_HandleFiring))
	{
		StartSimulateFire();
	}
	else if (BurstCount <= 0)
	{
		StopSimulateFire();
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, BurstCount, COND_SkipOwner);
}
