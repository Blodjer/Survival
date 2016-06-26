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

private:
	USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* Billboard;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = LandingZone, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AAirdropSupplyBox>> AirDropSupplies;

public:
	FORCEINLINE TArray<TSubclassOf<AAirdropSupplyBox>> GetAirDropSupplies() const { return AirDropSupplies; };

};
