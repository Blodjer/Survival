// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine.h"
#include "SurvivalStructs.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo)
	TSubclassOf<class AWeaponProjectile> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (ClampMin = "0.0", UIMin = "0.0"))
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

public:
	int32& operator [](TSubclassOf<class AWeaponProjectile> Type)
	{
		return AmmunitionCounts[AmmunitionTypes.IndexOfByKey(Type)];
	}

	int32 IndexOf(TSubclassOf<class AWeaponProjectile> Type) const
	{
		return AmmunitionTypes.IndexOfByKey(Type);
	}

	void AddAmmo(TSubclassOf<class AWeaponProjectile> Type, int32 Amount)
	{
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

	int32 const GetAmmoAmmountOfType(TSubclassOf<class AWeaponProjectile> Type) const
	{
		int32 i = AmmunitionTypes.IndexOfByKey(Type);
		if (i != INDEX_NONE)
		{
			return AmmunitionCounts[i];
		}

		return 0;
	}

	TArray<TSubclassOf<class AWeaponProjectile>> const GetAmmunitionTypes() const
	{
		return AmmunitionTypes;
	}

};
