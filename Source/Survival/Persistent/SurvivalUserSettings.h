// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/SaveGame.h"
#include "SurvivalUserSettings.generated.h"

UCLASS()
class SURVIVAL_API USurvivalUserSettings : public USaveGame
{
	GENERATED_BODY()	
	
public:
	USurvivalUserSettings();

	static USurvivalUserSettings* LoadSettings();

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SaveSettings();

public:
	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetToDefaults();

	UFUNCTION(BlueprintCallable, Category = Settings)
	void ApplyInputSettings();

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetMouseSensitivityX(float Value);

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetMouseSensitivityY(float Value);

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetInvertMouseY(bool bInvert);

public:
	UFUNCTION(BlueprintPure, Category = Settings)
	FORCEINLINE float GetMouseSensitivityX() const { return MouseSensitivityX; };

	UFUNCTION(BlueprintPure, Category = Settings)
	FORCEINLINE float GetMouseSensitivityY() const { return MouseSensitivityY; };

	UFUNCTION(BlueprintPure, Category = Settings)
	FORCEINLINE bool GetInvertMouseY() const { return bInvertMouseY; };

private:
	UPROPERTY()
	float MouseSensitivityX;

	UPROPERTY()
	float MouseSensitivityY;

	UPROPERTY()
	bool bInvertMouseY;

};
