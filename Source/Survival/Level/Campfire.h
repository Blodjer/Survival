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

	virtual void PostInitializeComponents() override;
	
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Campfire)
	virtual void SetActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = Campfire)
	bool IsCaptured();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Campfire)
	void UpdateCapturingPlayers();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CaptureSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Smoke, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SmokeParticleSystem;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Smoke, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* Audio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* DamageArea;

	UPROPERTY(BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	TArray<ASurvivalPlayerCharacter*> CapturingPlayers;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwningTeam)
	int32 OwningTeamIdx;

	UPROPERTY(Transient, Replicated)
	int32 DominantTeamIdx;

	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	FLinearColor OwnerBaseColor;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Campfire, meta = (ClampMin = "-1", UIMin = "-1", AllowPrivateAccess = "true"))
	int32 ClusterIdx;

	UPROPERTY(BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_IsActive, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	bool bIsActive;

	// The time it takes to capture and neutralize the campfire
	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (ClampMin = "0", UIMin = "0", AllowPrivateAccess = "true"))
	float CaptureDuration;

	// Multiplies to the capture duration for every additional team player
	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1", AllowPrivateAccess = "true"))
	float CaptureDurationMuliplier;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CaptureValue;

	UPROPERTY(BlueprintReadOnly, Transient, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	float CurrentCaptureDurationMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	FLinearColor SmokeBaseColor;

	UPROPERTY(EditDefaultsOnly, Category = Campfire)
	float FireDamage;

	UPROPERTY(Transient)
	TArray<APawn*> DamagingPawns;

private:
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnCapturingPlayersChanged();

	UFUNCTION()
	void OnBeginDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UFUNCTION()
	void OnRep_OwningTeam();

	UFUNCTION()
	void OnRep_IsActive();

public:
	FORCEINLINE USphereComponent* GetCaptureSphere() const { return CaptureSphere; }

	FORCEINLINE UParticleSystemComponent* GetSmokeParticleSystem() const { return SmokeParticleSystem; }

	FORCEINLINE float GetCaptureValue() const { return CaptureValue; }

	FORCEINLINE FLinearColor GetOwnerBaseColor() const { return OwnerBaseColor; }

	FORCEINLINE int32 GetOwningTeamIdx() const { return OwningTeamIdx; }

	FORCEINLINE int32 GetClusterIdx() const { return ClusterIdx; }
};
