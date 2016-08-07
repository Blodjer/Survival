// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "DroppablePhysicsActor.generated.h"

UCLASS()
class SURVIVAL_API ADroppablePhysicsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADroppablePhysicsActor();

	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnRep_ReplicateMovement() override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Physics)
	virtual void StartSimulatePhysics(FVector Velocity = FVector::ZeroVector, FVector AngularVelocity = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Physics)
	virtual void StopSimulatePhysics();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Physics)
	FName SimulatingPhysicsProfileName;

	UPROPERTY(EditDefaultsOnly, Category = Physics)
	bool bDropWithCCD;

private:
	UPrimitiveComponent* PhysicsRootComponent;

	FName DefaultPhysicProfileName;

};
