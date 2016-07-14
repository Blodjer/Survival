// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Survival/Level/AirdropSupplyBox.h"
#include "AirdropLandingZone.generated.h"

UCLASS()
class SURVIVAL_API AAirdropLandingZone : public AActor
{
	GENERATED_BODY()
	
public:
	AAirdropLandingZone();

	virtual void BeginPlay() override;

#if WITH_EDITOR
	UPROPERTY(Transient, EditAnywhere, Category = Debug)
	bool bShowHelper;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

	UFUNCTION(BlueprintPure, Category = LandingZone)
	void GetRandomApproachLocation(float DropHeight, FVector& StartLocation, FVector& LandingLocation) const;

private:
	USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* Billboard;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AAirdropSupplyBox>> AirDropSupplies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LandingZone, meta = (ClampMin = "20.0", UIMin = "0.0", AllowPrivateAccess = "true"))
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LandingZone, meta = (ClampMin = "20.0", UIMin = "20.0", ClampMax = "90.0", UIMax = "90.0", AllowPrivateAccess = "true"))
	float ApproachAngle;

public:
	FORCEINLINE TArray<TSubclassOf<AAirdropSupplyBox>> GetAirDropSupplies() const { return AirDropSupplies; };

};
