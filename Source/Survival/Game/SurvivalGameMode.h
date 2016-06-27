// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/GameMode.h"
#include "../SurvivalStructs.h"
#include "SurvivalGameMode.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FAirdropSupply
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner)
	TSubclassOf<AAirdropSupplyBox> SupplyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawner)
	UCurveFloat* ProbabilityCurve;

	FORCEINLINE float GetProbability(float Time)
	{
		return ProbabilityCurve ? ProbabilityCurve->GetFloatValue(Time) : 1.0f;
	}

	FAirdropSupply()
	{
		
	}

	FAirdropSupply(TSubclassOf<AAirdropSupplyBox> SupplyClass, UCurveFloat* ProbabilityCurve)
	{
		this->SupplyClass = SupplyClass;
		this->ProbabilityCurve = ProbabilityCurve;
	}
};

UCLASS()
class SURVIVAL_API ASurvivalGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ASurvivalGameMode();

	virtual void PostInitializeComponents() override;

	virtual void InitGameState() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual void Tick(float DeltaTime) override;

	virtual void StartMatch() override;

public:
	void Killed(const UDamageType* DamageType, AController* Killer, AController* KilledPlayer);

	void RegisterCampfire(class ACampfire* Campfire);

	void RegisterAirdropLandingZone(class AAirdropLandingZone* LandingZone);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameMode, meta = (DisplayName = "Minimum Die Delay"))
	float MinDieDelay;

protected:
	virtual void DetermineActiveCampfireCluster();

	virtual bool DetermineMatchWinner(int32& WinnerTeamIdx);

	virtual void DetermineNextAirdrop();
	
	UFUNCTION(BlueprintCallable, Category = Airdrop)
	void SendAirdrop(TSubclassOf<class AAirdropSupplyBox> Payload, class AAirdropLandingZone* LandingZone);

protected:
	// The length of one day in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = TimeOfDay)
	float LengthOfDay;

	// At what time the match should start (0-24)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "24.0", UIMin = "0.0", UIMax = "24.0"), Category = TimeOfDay)
	float StartTimeOfDay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Teams)
	TArray<FTeamInfo> Teams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop)
	TSubclassOf<class AAirdrop> AirdropClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Airdrop)
	TArray<FAirdropSupply> AirdropSupplies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Airdrop)
	FFloatSpan AirdropInterval;

	UPROPERTY(BlueprintReadOnly, Category = GameMode)
	TArray<class ACampfire*> Campfires;

	UPROPERTY(BlueprintReadOnly, Category = GameMode)
	TArray<class ACampfire*> ActiveCampfires;

	UPROPERTY(BlueprintReadOnly, Category = GameMode)
	TArray<class AAirdropLandingZone*> AirdropLandingZones;

private:
	UPROPERTY(BlueprintReadOnly, Category = GameMode, meta = (AllowPrivateAccess = "true"))
	int32 ActiveCampfireCluster;

	TMap<TSubclassOf<AAirdropSupplyBox>, int32> SuppliesProbabilityModifier;

	FTimerHandle TimerHandle_SendAirdrop;
	
};
