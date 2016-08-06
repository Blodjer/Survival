// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Components/ActorComponent.h"
#include "../SurvivalTypes.h"
#include "PickupSpawnerComponent.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FPickupSpawnerItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner)
	TSubclassOf<APickup> Pickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner)
	float Probability;

	FPickupSpawnerItem()
	{
		Probability = 1.0f;
	}

	FPickupSpawnerItem(TSubclassOf<APickup> Pickup, float Probability)
	{
		this->Pickup = Pickup;
		this->Probability = Probability;
	}
};

UCLASS(Blueprintable, ClassGroup = (Spawner), meta = (BlueprintSpawnableComponent))
class SURVIVAL_API UPickupSpawnerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPickupSpawnerComponent();

#if WITH_EDITORONLY_DATA
	virtual void OnRegister() override;
#endif

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	void UpdateVisualizeSprite();
	void UpdateVisualizeBox();
#endif

protected:
	FORCEINLINE bool IsRespawnable() const { return RespawnTime.Min > 1.0f && RespawnTime.Max > 1.0f;  }

private:
	void Spawn();

	void TryRespawn();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner, meta = (AllowPrivateAccess = "true"))
	TArray<FPickupSpawnerItem> SpawnList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner, meta = (AllowPrivateAccess = "true"))
	FFloatSpan RespawnTime;

	FTimerHandle TimerHandle_Respawn;

	UPROPERTY(Transient)
	APickup* SpawnedPickup;

};
