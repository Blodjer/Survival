// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class SURVIVAL_API APickup : public AActor
{
	GENERATED_BODY()
	
public:
	APickup();

	UFUNCTION()
	void Pickup(ASurvivalPlayerCharacter* PlayerCharacter);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = Pickup)
	void OnPickup(ASurvivalPlayerCharacter* PlayerCharacter);
	virtual void OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter);

	UFUNCTION(NetMulticast, Unreliable)
	void SimulatePickup();
	void SimulatePickup_Implementation();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PickupMesh;

	// Name displayed when a player aim's at the Pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	FName PickupName;

public:
	FORCEINLINE UStaticMeshComponent* GetPickupMesh() const { return PickupMesh; };

};
