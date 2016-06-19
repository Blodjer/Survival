// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class SURVIVAL_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawner, meta = (AllowPrivateAccess = "true"))
	class UPickupSpawnerComponent* PickupSpawnerComponent;
};
