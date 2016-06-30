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
	virtual void Tick(float DeltaSeconds) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

	UFUNCTION(BlueprintPure, Category = LandingZone)
	FVector GetRandomLandingLocation() const;

private:
	USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* Billboard;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AAirdropSupplyBox>> AirDropSupplies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LandingZone, meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = "true"))
	float Radius;

public:
	FORCEINLINE TArray<TSubclassOf<AAirdropSupplyBox>> GetAirDropSupplies() const { return AirDropSupplies; };

};
