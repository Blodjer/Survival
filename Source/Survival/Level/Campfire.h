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

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CaptureSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Smoke, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SmokeParticleSystem;

	UPROPERTY(BlueprintReadOnly, Category = Campfire, meta = (AllowPrivateAccess = "true"))
	TArray<ASurvivalPlayerCharacter*> CapturingPlayers;

	FTeamInfo OwningTeam;

private:
	UFUNCTION()
	void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnCapturingPlayersChanged();

public:
	FORCEINLINE USphereComponent* GetCaptureSphere() const { return CaptureSphere; }

	FORCEINLINE UParticleSystemComponent* GetSmokeParticleSystem() const { return SmokeParticleSystem; }

};
