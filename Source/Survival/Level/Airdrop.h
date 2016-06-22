// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Components/InterpToMovementComponent.h"
#include "../Plugins/Runtime/CableComponent/Source/CableComponent/Classes/CableComponent.h"
#include "Airdrop.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AAirdrop : public AActor
{
	GENERATED_BODY()
	
public:
	AAirdrop();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = Airdrop)
	virtual void Init(FVector LandingLocation);

protected:
	UFUNCTION()
	void OnLanded(const FHitResult& ImpactResult, float Time);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Payload)
	void ReleasePayload();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Payload)
	void StartEscapeRoute();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Payload)
	TSubclassOf<ADroppablePhysicsActor> PayloadClass;

	UPROPERTY(BlueprintReadOnly, Category = Payload)
	ADroppablePhysicsActor* Payload;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BalloonMesh;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* PayloadCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	UInterpToMovementComponent* MovementComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable1;

	UPROPERTY(VisibleDefaultsOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable2;

	UPROPERTY(VisibleDefaultsOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable3;

	UPROPERTY(VisibleDefaultsOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable4;

	UPROPERTY(BlueprintReadOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	TArray<UCableComponent*> CableComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	float EscapeDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	float EscapeSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop, meta = (AllowPrivateAccess = "true"))
	float ReleasePayloadHeight;

	FVector StartLocation;

	FVector LandingLocation;

};
