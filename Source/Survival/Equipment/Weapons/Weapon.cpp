// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Weapon.h"


AWeapon::AWeapon()
{
	Type = EHandheldType::PrimaryWeapon;

	Damage = 25.0f;

	RateOfFire = 750;

	RecoilUpMin = 0.45f;
	RecoilUpMax = 0.65f;
	RecoilLeft = 0.35f;
	RecoilRight = 0.35f;
	RecoilFirstShotMultiplier = 1.4f;

	IronSightLocation = FVector(0.0f, -5.0f, 10.0f);
	IronSightCameraDistance = 20.0f;
	IronSightZoom = 1.0f;

	MaxRoundsPerMagazine = 20;
	CurrentRoundsInMagazine = 0;

	bAutomatic = false;
	bBurst = false;
	bSemiAutomatic = true;

	bIsAiming = false;

	NoAnimReloadDuration = 1.5f;

	BurstCount = 0;

	bIsReloading = false;

	LastShotTime = FLT_MIN;

	CurrentSpread = 0.0f;
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FireMode = GetBestFireMode();
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (LastShotTime + 60.0f / RateOfFire <= GetWorld()->GetTimeSeconds())
	{
		CurrentSpread = FMath::Max(CurrentSpread - GetSpreadDecrease() * DeltaSeconds, 0.0f);
	}
}

void AWeapon::UnEquip()
{
	StopFire();
	StopAiming();

	Super::UnEquip();
}

void AWeapon::OnCharacterStopUse()
{
	Super::OnCharacterStopUse();

	if (HasAuthority() || (GetOwnerCharacter() && GetOwnerCharacter()->IsLocallyControlled()))
	{
		StopFire();
		StopAiming();
		StopReload();
	}
}

void AWeapon::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("Fire", IE_Pressed, this, &AWeapon::StartFire);
	BindInputAction("Fire", IE_Released, this, &AWeapon::StopFire);

	BindInputAction("Aim", IE_Pressed, this, &AWeapon::StartAiming);
	BindInputAction("Aim", IE_Released, this, &AWeapon::StopAiming);

	BindInputAction("Reload", IE_Pressed, this, &AWeapon::StartReload);

	BindInputAction("SwitchFireMode", IE_Pressed, this, &AWeapon::SwitchFireMode);
}

void AWeapon::BeforeDrop()
{
	if (GetOwnerCharacter() == nullptr && ProjectileType != nullptr)
		return;

	GetOwnerCharacter()->AddAmmo(ProjectileType, GetCurrentRoundsInMagazine());	
}

void AWeapon::StartFire()
{
	if (!CanFire())
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
	if (FireMode == EFireMode::Burst && BurstCount < 3)
		return;

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
		if (CurrentRoundsInMagazine > 0 &&
			(FireMode == EFireMode::Automatic ||
			(FireMode == EFireMode::Burst && BurstCount < 3) ||
			(FireMode == EFireMode::SemiAutomatic && BurstCount < 1)))
		{
			ShootProjectile();

			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::HandleFiring, 60.0f / RateOfFire, false);
		}
		else
		{
			StopFire();
		}
	}
}

void AWeapon::ShootProjectile()
{
	FVector Direction = FMath::VRandCone(GetOwnerCharacter()->GetBaseAimRotation().Vector(), FMath::DegreesToRadians(GetSpreadBase() + CurrentSpread));
	ServerShootProjectile(GetOwnerCharacter()->GetCamera()->GetComponentLocation(), Direction);

	if (!GetWorld()->IsServer())
	{
		SimulateFire();

		CurrentRoundsInMagazine--;
		BurstCount++;

		LastShotTime = GetWorld()->GetTimeSeconds();
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetOwnerCharacter()->GetController());
	if (PlayerController)
	{
		float RecoilMultiplier = BurstCount == 1 ? RecoilFirstShotMultiplier : 1.0f;
		FRotator NewControlRotation = PlayerController->GetControlRotation().Add(FMath::FRandRange(RecoilUpMin * RecoilMultiplier, RecoilUpMax * RecoilMultiplier), FMath::FRandRange(-RecoilLeft, RecoilRight), 0.0f);
		PlayerController->SetControlRotation(NewControlRotation);
	}

	CurrentSpread = FMath::Min(CurrentSpread + GetSpreadIncrease(), GetSpreadMax());
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

	LastShotTime = GetWorld()->GetTimeSeconds();
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

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, GetActorLocation());

	// TODO: Remove
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetOwnerCharacter()->GetBaseAimRotation().Vector() * 100000.0f, FColor::White, false, 0.15f);

	OnSimulateFire();
}

void AWeapon::StartAiming()
{
	if (!CanAim())
		return;

	GetOwnerCharacter()->GetCamera()->SetFieldOfView(90.0f / IronSightZoom);

	GetMesh1P()->AttachToComponent(GetOwnerCharacter()->GetCamera(), FAttachmentTransformRules::KeepWorldTransform);

	FRotator WeaponBaseRotation = FRotator(0.0f, -90.0f, 0.0f);
	GetMesh1P()->SetRelativeLocationAndRotation(WeaponBaseRotation.RotateVector(IronSightLocation) * FVector(1,-1,-1) + FVector(IronSightCameraDistance, 0.0f, 0.0f), WeaponBaseRotation);

	GetOwnerCharacter()->GetMesh1P()->SetVisibility(false);

	GetWorld()->Exec(GetWorld(), TEXT("SetMouseSensitivity 0.04"));

	bIsAiming = true;
}

void AWeapon::StopAiming()
{
	if (GetOwnerCharacter() == nullptr)
		return;

	GetOwnerCharacter()->GetCamera()->SetFieldOfView(90.0f);

	GetMesh1P()->AttachToComponent(GetOwnerCharacter()->GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");

	GetOwnerCharacter()->GetMesh1P()->SetVisibility(true);

	GetWorld()->Exec(GetWorld(), TEXT("SetMouseSensitivityToDefault"));

	bIsAiming = false;
}

void AWeapon::StartReload()
{
	if (!CanReload())
		return;

	if (!GetOwnerCharacter()->IsLocallyControlled() && !HasAuthority())
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

	if (GetOwnerCharacter() == nullptr)
		return;

	if (HasAuthority() || GetOwnerCharacter()->IsLocallyControlled())
	{
		CurrentRoundsInMagazine = FMath::Clamp(CurrentRoundsInMagazine + GetOwnerCharacter()->RequestAmmo(ProjectileType, MaxRoundsPerMagazine - CurrentRoundsInMagazine + (CurrentRoundsInMagazine > 0 ? 1 : 0)), 0, MaxRoundsPerMagazine + 1);
	}

	if (GetOwnerCharacter()->IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, "Reload Complete");
	}
}

void AWeapon::StopReload()
{
	if (!bIsReloading)
		return;

	GetWorldTimerManager().ClearTimer(TimerHandle_Reload);

	bIsReloading = false;
}

void AWeapon::SwitchFireMode()
{
	if (bAutomatic + bBurst + bSemiAutomatic <= 1)
		return;

	if (GetOwnerCharacter() == nullptr)
		return;

	if (!HasAuthority() && GetOwnerCharacter()->IsLocallyControlled())
	{
		ServerSwitchFireMode();
	}

	for (uint8 i = 1; i < 2; i++)
	{
		EFireMode NextFireMode = (EFireMode)(((uint8)FireMode + i) % 3);
		if (IsValidFireMode(NextFireMode))
		{
			FireMode = NextFireMode;
			break;
		}
	}
}

void AWeapon::ServerSwitchFireMode_Implementation()
{
	SwitchFireMode();
}

void AWeapon::SimulateSwitchFireMode()
{
	// TODO: Play Sound
}

EFireMode AWeapon::GetBestFireMode()
{
	if (bAutomatic)
		return EFireMode::Automatic;
	if (bBurst)
		return EFireMode::Burst;
	if (bSemiAutomatic)
		return EFireMode::SemiAutomatic;

	return EFireMode::SemiAutomatic;
}

bool AWeapon::IsValidFireMode(EFireMode FireMode)
{
	if (FireMode == EFireMode::Automatic && bAutomatic)
		return true;
	if (FireMode == EFireMode::Burst && bBurst)
		return true;
	if (FireMode == EFireMode::SemiAutomatic && bSemiAutomatic)
		return true;

	return false;
}

bool AWeapon::CanFire()
{
	if (GetWorld() == nullptr || GetOwnerCharacter() == nullptr)
		return false;

	return LastShotTime + 60.0f / RateOfFire <= GetWorld()->GetTimeSeconds() && !bIsReloading && !GetOwnerCharacter()->IsSprinting();
}

bool AWeapon::CanAim()
{
	return GetOwnerCharacter() && !GetOwnerCharacter()->IsSprinting();
}

bool AWeapon::CanReload()
{
	return GetOwnerCharacter() && !bIsReloading && !GetOwnerCharacter()->IsSprinting();
}

float AWeapon::GetSpreadBase() const
{
	if (SpreadDataTable == nullptr)
		return 0.0f;

	FWeaponMovementValues* Row = SpreadDataTable->FindRow<FWeaponMovementValues>("Base", "Base");
	return Row ? Row->GetValue(GetOwnerCharacter(), bIsAiming) : 0.0f;
}

float AWeapon::GetSpreadMax() const
{
	if (SpreadDataTable == nullptr)
		return 0.0f;

	FWeaponMovementValues* Row = SpreadDataTable->FindRow<FWeaponMovementValues>("Max", "Max");
	return Row ? Row->GetValue(GetOwnerCharacter(), bIsAiming) : 0.0f;
}

float AWeapon::GetSpreadIncrease() const
{
	if (SpreadDataTable == nullptr)
		return 0.0f;

	FWeaponMovementValues* Row = SpreadDataTable->FindRow<FWeaponMovementValues>("Increase", "Increase");
	return Row ? Row->GetValue(GetOwnerCharacter(), bIsAiming) : 0.0f;
}

float AWeapon::GetSpreadDecrease() const
{
	if (SpreadDataTable == nullptr)
		return 0.0f;

	FWeaponMovementValues* Row = SpreadDataTable->FindRow<FWeaponMovementValues>("Decrease", "Decrease");
	return Row ? Row->GetValue(GetOwnerCharacter(), bIsAiming) : 0.0f;
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
	else
	{
		StopReload();
	}
}

void AWeapon::OnRep_FireMode()
{
	SimulateSwitchFireMode();
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, BurstCount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeapon, bIsReloading, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AWeapon, CurrentRoundsInMagazine, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AWeapon, FireMode, COND_SkipOwner);
}
