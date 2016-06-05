// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Equipment/Handheld.h"
#include "Gadget.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AGadget : public AHandheld
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputActions() override;

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

protected:
	UPROPERTY(EditDefaultsOnly, Category = Gadget)
	bool bIsDisposable;
	
};
