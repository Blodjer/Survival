// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Campfire.generated.h"

UCLASS()
class SURVIVAL_API ACampfire : public AActor
{
	GENERATED_BODY()
	
public:	
	ACampfire();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CaptureSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Smoke, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SmokeParticleSystem;

	UPROPERTY(BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	TArray<ASurvivalPlayerCharacter*> CapturingPlayers;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwningTeam)
	int32 OwningTeamIdx;

	UPROPERTY(Transient, Replicated)
	int32 DominantTeamIdx;

	UPROPERTY(Transient)
	FLinearColor CurrentOwnerBaseColor;

	// The time it takes to capture and neutralize the campfire
	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CaptureDuration;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CaptureValue;

	UPROPERTY(EditDefaultsOnly, Category = Campfire)
	FLinearColor SmokeBaseColor;

private:
	UFUNCTION()
	void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnCapturingPlayersChanged();

private:
	UFUNCTION()
	void OnRep_OwningTeam();

public:
	FORCEINLINE USphereComponent* GetCaptureSphere() const { return CaptureSphere; }

	FORCEINLINE UParticleSystemComponent* GetSmokeParticleSystem() const { return SmokeParticleSystem; }

	FORCEINLINE float GetCaptureValue() const { return CaptureValue; }

};
