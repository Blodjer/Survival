// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Weapon.h"


AWeapon::AWeapon()
{
	CurrentState = EWeaponState::Idle;

	Damage = 25.0f;

	RateOfFire = 750;

	RecoilUpMin = 0.45f;
	RecoilUpMax = 0.65f;
	RecoilLeft = 0.35f;
	RecoilRight = 0.35f;

	Spread = 0.5f;

	MaxRoundsPerMagazine = 20;
	CurrentRoundsInMagazine = 0;

	NoAnimReloadDuration = 1.5f;

	BurstCount = 0;

	bIsReloading = false;
}

void AWeapon::UnEquip()
{
	StopFire();

	Super::UnEquip();
}

void AWeapon::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("Fire", IE_Pressed, this, &AWeapon::StartFire);
	BindInputAction("Fire", IE_Released, this, &AWeapon::StopFire);

	BindInputAction("Reload", IE_Pressed, this, &AWeapon::StartReload);

	// TODO: Zoom

	// TODO: Firemode
}

void AWeapon::BeforeDrop()
{
	if (GetOwnerCharacter() == nullptr && ProjectileType != nullptr)
		return;

	GetOwnerCharacter()->AddAmmo(ProjectileType, GetCurrentRoundsInMagazine());	
}

void AWeapon::StartFire()
{
	if (bIsReloading)
		return;

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
		if (CurrentRoundsInMagazine > 0)
		{
			ShootProjectile();
		}
		else
		{
			StopFire();
		}
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::HandleFiring, 60.0f / RateOfFire, false);
}

void AWeapon::ShootProjectile()
{
	FVector Direction = FMath::VRandCone(GetOwnerCharacter()->GetBaseAimRotation().Vector(), FMath::DegreesToRadians(Spread));
	ServerShootProjectile(GetOwnerCharacter()->GetCamera()->GetComponentLocation(), Direction);

	if (!GetWorld()->IsServer())
	{
		SimulateFire();

		CurrentRoundsInMagazine--;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetOwnerCharacter()->GetController());
	if (PlayerController)
	{
		FRotator NewControlRotation = PlayerController->GetControlRotation().Add(FMath::FRandRange(RecoilUpMin, RecoilUpMax), FMath::FRandRange(-RecoilLeft, RecoilRight), 0.0f);
		PlayerController->SetControlRotation(NewControlRotation);
	}
}

void AWeapon::ServerShootProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal Direction)
{
	if (ProjectileType == nullptr || GetWorld() == nullptr)
		return;

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Instigator = Instigator;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponProjectile* NewProjectile = GetWorld()->SpawnActor<AWeaponProjectile>(ProjectileType, Origin, Direction.Rotation(), SpawnParameters);
	if (NewProjectile)
	{
		NewProjectile->InitProjectile(Direction);

		BurstCount++;
		CurrentRoundsInMagazine--;

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

	// TODO: Remove
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetOwnerCharacter()->GetBaseAimRotation().Vector() * 100000.0f, FColor::White, false, 0.15f);

	OnSimulateFire();
}

void AWeapon::StartReload()
{
	if (GetOwnerCharacter() == nullptr)
		return;

	if (bIsReloading && (GetOwnerCharacter()->IsLocallyControlled() && !HasAuthority()))
		return;

	if (GetOwnerCharacter()->IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "Reload...");
	}

	if (!HasAuthority() && GetOwnerCharacter()->IsLocallyControlled())
	{
		ServerStartReload();
	}

	StopFire();

	GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &AWeapon::Reload, NoAnimReloadDuration, false);

	bIsReloading = true;
}

void AWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

void AWeapon::Reload()
{
	bIsReloading = false;

	if (GetOwnerCharacter()->IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "Reload Complete");
	}

	if (GetOwnerCharacter() == nullptr)
		return;

	if (HasAuthority() || GetOwnerCharacter()->IsLocallyControlled())
	{
		CurrentRoundsInMagazine = FMath::Clamp(CurrentRoundsInMagazine + GetOwnerCharacter()->RequestAmmo(ProjectileType, MaxRoundsPerMagazine - CurrentRoundsInMagazine), 0, MaxRoundsPerMagazine);
	}
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

void AWeapon::OnRep_Reload()
{
	if (bIsReloading)
	{
		StartReload();
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, BurstCount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeapon, bIsReloading, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AWeapon, CurrentRoundsInMagazine, COND_OwnerOnly);	
}
