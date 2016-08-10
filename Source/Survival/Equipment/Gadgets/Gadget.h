// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Equipment/Handheld.h"
#include "Gadget.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AGadget : public AHandheld
{
	GENERATED_BODY()

public:
	AGadget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void SetupInputActions() override;

	virtual void OnCharacterStopUse() override;

	void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse();
	void ServerUse_Implementation();
	bool ServerUse_Validate() { return true; };

	UFUNCTION(BlueprintNativeEvent, Category = Gadget)
	void OnUse();
	virtual void OnUse_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void SimulateUse();
	void SimulateUse_Implementation();

	UFUNCTION()
	void CooldownEnd();

protected:
	virtual void BeforeDrop() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Gadget)
	float UseDelay;

	UPROPERTY(EditDefaultsOnly, Category = Gadget)
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, Category = Gadget)
	FHandheldAnim UseAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Gadget)
	bool bIsDisposable;
	
private:
	UPROPERTY(Transient)
	bool bIsInUse;

	FTimerHandle TimerHandle_UseDelay;

	FTimerHandle TimerHandle_Cooldown;

};
