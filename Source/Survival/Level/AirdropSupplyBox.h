// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Level/SupplyBox.h"
#include "AirdropSupplyBox.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AAirdropSupplyBox : public ASupplyBox
{
	GENERATED_BODY()
	
public:
	AAirdropSupplyBox();

	virtual void Open() override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = SupplyBox, meta = (AllowPrivateAccess = "true"))
	class UFlashingPointLightComponent* LightSignal;
	
};
