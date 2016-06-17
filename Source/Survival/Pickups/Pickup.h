// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Survival/Components/DroppablePhysicsActor.h"
#include "Pickup.generated.h"

UCLASS()
class SURVIVAL_API APickup : public ADroppablePhysicsActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	APickup();

	virtual void Interact_Implementation(ASurvivalPlayerCharacter* PlayerCharacter) override;

	virtual bool IsInteractable_Implementation(ASurvivalPlayerCharacter* PlayerCharacter) override;

	FORCEINLINE virtual FName IInteractable::GetInteractableName_Implementation() const override { return PickupName; };

	FORCEINLINE virtual FString IInteractable::GetInteractionVerb_Implementation() const override { return "Pickup"; };

	void Pickup(ASurvivalPlayerCharacter* PlayerCharacter);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = Pickup)
	void OnPickup(ASurvivalPlayerCharacter* PlayerCharacter);
	virtual void OnPickup_Implementation(ASurvivalPlayerCharacter* PlayerCharacter);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCosmetic)
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
