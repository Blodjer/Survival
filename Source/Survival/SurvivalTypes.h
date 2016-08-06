// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine.h"
#include "SurvivalTypes.generated.h"

/*
	DEFINES
*/

#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Wood		SurfaceType1
#define SURFACE_Stone		SurfaceType2
#define SURFACE_Dirt		SurfaceType3
#define SURFACE_Grass		SurfaceType4
#define SURFACE_Water		SurfaceType5
#define SURFACE_Flesh		SurfaceType6
#define SURFACE_Metal		SurfaceType7
#define SURFACE_Floor		SurfaceType8

/*
	STRUCTS
*/

USTRUCT(BlueprintType)
struct SURVIVAL_API FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	FLinearColor Color;

	FTeamInfo()
	{
		Name = "Unknown";
		Color = FColor(15, 15, 15);
	}

	FTeamInfo(FName Name, FLinearColor Color)
	{
		this->Name = Name;
		this->Color = Color;
	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FAmmo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	TSubclassOf<class AWeaponProjectile> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (ClampMin = "0.0", UIMin = "0.0"))
	int32 Amount;

	FAmmo()
	{
		Amount = 0;
	}

	FAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount)
	{
		this->Type = Type;
		this->Amount = Amount;
	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FAmmunitionInventory
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<TSubclassOf<class AWeaponProjectile>> AmmunitionTypes;

	UPROPERTY()
	TArray<int32> AmmunitionCounts;

	UPROPERTY(EditAnywhere, NotReplicated, Category = Ammunition, meta = (AllowPrivateAccess = "true"))
	TArray<FAmmo> MaxAmmunition;

public:
	int32& operator [](TSubclassOf<class AWeaponProjectile> Type)
	{
		return AmmunitionCounts[AmmunitionTypes.IndexOfByKey(Type)];
	}

	int32 IndexOf(TSubclassOf<class AWeaponProjectile> Type) const
	{
		return AmmunitionTypes.IndexOfByKey(Type);
	}

	int32 AddAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount, bool bOverfill = false)
	{
		if (!bOverfill)
		{
			const FAmmo* MaxAmmo = MaxAmmunition.FindByPredicate([Type](FAmmo& Ammo) { return Ammo.Type == Type; });
			Amount = MaxAmmo ? FMath::Min(FMath::Max(MaxAmmo->Amount - GetAmmoAmountOfType(Type), 0), Amount) : Amount;
		}

		if (Amount <= 0)
			return Amount;

		int32 i = IndexOf(Type);
		if (i != INDEX_NONE)
		{
			AmmunitionCounts[i] += Amount;
		}
		else
		{
			AmmunitionTypes.Add(Type);
			AmmunitionCounts.Add(Amount);
		}

		return Amount;
	}

	int32 RequestAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount)
	{
		int32 i = IndexOf(Type);
		if (i != INDEX_NONE)
		{
			int32 GiveAmount = FMath::Min(AmmunitionCounts[i], Amount);
			AmmunitionCounts[i] -= GiveAmount;
			return GiveAmount;
		}

		return 0;
	}

	int32 const GetAmmoAmountOfType(TSubclassOf<class AWeaponProjectile> Type) const
	{
		int32 i = IndexOf(Type);
		if (i != INDEX_NONE)
		{
			return AmmunitionCounts[i];
		}

		return 0;
	}

	int32 const GetRemainingAmmoAmountOfType(TSubclassOf<class AWeaponProjectile> Type) const
	{
		const FAmmo* MaxAmmo = MaxAmmunition.FindByPredicate([Type](FAmmo& Ammo) { return Ammo.Type == Type; });
		return MaxAmmo ? FMath::Max(MaxAmmo->Amount - GetAmmoAmountOfType(Type), 0) : INT32_MAX;
	}

	TArray<TSubclassOf<class AWeaponProjectile>> const GetAmmunitionTypes() const
	{
		return AmmunitionTypes;
	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FHitZone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HitZone)
	FName ParentBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HitZone)
	float DamageMultiplier;

	FHitZone() : DamageMultiplier(1.0f)
	{

	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FFloatSpan
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = FloatSpan)
	float Min;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = FloatSpan)
	float Max;

	FFloatSpan() : Min(0.0f), Max(0.0f)
	{
		
	}

	FFloatSpan(float Min, float Max)
	{
		this->Min = Min;
		this->Max = Max;
	}

	FORCEINLINE float Random() const
	{
		return FMath::RandRange(Min, Max);
	}
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FPhysicalVFX
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Default;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Wood;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Stone;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Dirt;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Grass;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Water;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Flesh;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* Metal;
};

USTRUCT(BlueprintType)
struct SURVIVAL_API FPhysicalSFX
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Default;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Wood;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Stone;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Dirt;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Grass;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Water;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Flesh;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* Metal;
};
