// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Components/PointLightComponent.h"
#include "FlashingPointLightComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Lights), meta = (BlueprintSpawnableComponent))
class SURVIVAL_API UFlashingPointLightComponent : public UPointLightComponent
{
	GENERATED_BODY()

public:
	UFlashingPointLightComponent();

	virtual void OnRegister() override;

	UFUNCTION(BlueprintCallable, Category = Light)
	void SetFlashingPattern(TArray<float> Pattern);

	void SetFlashingPattern(float FlashTime);

	UFUNCTION(BlueprintCallable, Category = Light)
	void StartOn();

	UFUNCTION(BlueprintCallable, Category = Light)
	void StopOff();

private:
	void ToggleLight();

private:
	UPROPERTY(EditAnywhere, Category = Light)
	TArray<float> FlashingPattern;
	
	int32 CurrentPatternIndex;

	UPROPERTY(EditAnywhere, Category = Light)
	bool StartWithLightOn;

	FTimerHandle TimerHandle_ToggleLight;
	
};
