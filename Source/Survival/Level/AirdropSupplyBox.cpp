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

	//GetCaseMesh()->SetMassOverrideInKg(NAME_None, 150.0f, true);
	//GetCaseMesh()->SetCenterOfMass(FVector(0, 0, -30.0f));

	bDropWithCCD = true;

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
