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

public:
	UFUNCTION(BlueprintPure, Category = Campfire)
	bool IsCaptured();

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

	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	FLinearColor OwnerBaseColor;

	// The time it takes to capture and neutralize the campfire
	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float CaptureDuration;

	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1", AllowPrivateAccess = "true"))
	float CaptureDurationMuliplier;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CaptureValue;

	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CurrentCaptureDurationMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = Campfire)
	FLinearColor SmokeBaseColor;

private:
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnCapturingPlayersChanged();

private:
	UFUNCTION()
	void OnRep_OwningTeam();

public:
	FORCEINLINE USphereComponent* GetCaptureSphere() const { return CaptureSphere; }

	FORCEINLINE UParticleSystemComponent* GetSmokeParticleSystem() const { return SmokeParticleSystem; }

	FORCEINLINE float GetCaptureValue() const { return CaptureValue; }

	FORCEINLINE FLinearColor GetOwnerBaseColor() const { return OwnerBaseColor; }

	FORCEINLINE int32 GetOwningTeamIdx() const { return OwningTeamIdx; }
};
