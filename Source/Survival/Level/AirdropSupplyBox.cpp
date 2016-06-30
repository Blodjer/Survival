// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "AirdropSupplyBox.h"
#include "Components/FlashingPointLightComponent.h"

AAirdropSupplyBox::AAirdropSupplyBox()
{
	LightSignal = CreateDefaultSubobject<UFlashingPointLightComponent>("LightSignal");
	LightSignal->LightColor = FColor::Red;
	LightSignal->Intensity = 6000.0f;
	LightSignal->AttenuationRadius = 1650.0f;
	LightSignal->SetFlashingPattern(0.7f);
	LightSignal->SetupAttachment(RootComponent);

	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = true;
}

void AAirdropSupplyBox::Open()
{
	Super::Open();

	LightSignal->StopOff();
}

void AAirdropSupplyBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Actor : AttachedActors)
	{
		Actor->Destroy(true);
	}

	Super::EndPlay(EndPlayReason);
}
