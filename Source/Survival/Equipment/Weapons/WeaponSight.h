// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "WeaponAttachment.h"
#include "WeaponSight.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FWeaponSightConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Sight, meta = (MakeEditWidget = true))
	FVector CenterLocation;

	UPROPERTY(EditDefaultsOnly, Category = Sight, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CameraDistance;

	UPROPERTY(EditDefaultsOnly, Category = Sight, meta = (ClampMin = "1.0", UIMin = "1.0"))
	float ZoomFactor;

	FWeaponSightConfig() : CenterLocation(FVector::ZeroVector), CameraDistance(15.0f), ZoomFactor(1.2f)
	{

	}

	FWeaponSightConfig(FVector Location, float CameraDistance, float ZoomFactor)
	{
		this->CenterLocation = Location;
		this->CameraDistance = CameraDistance;
		this->ZoomFactor = ZoomFactor;
	}
};

UCLASS(Abstract)
class SURVIVAL_API AWeaponSight : public AWeaponAttachment
{
	GENERATED_BODY()
	
public:
	AWeaponSight();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sight, meta = (AllowPrivateAccess = "true"))
	FWeaponSightConfig Config;

public:
	FORCEINLINE FWeaponSightConfig GetConfig() const { return Config; };

};
