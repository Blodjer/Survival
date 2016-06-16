// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "FlashingPointLightComponent.h"

UFlashingPointLightComponent::UFlashingPointLightComponent()
{
	FlashingPattern.Add(0.7f);
	FlashingPattern.Add(0.7f);

	CurrentPatternIndex = 0;

	bWantsInitializeComponent = true;
}

void UFlashingPointLightComponent::OnRegister()
{
	Super::OnRegister();

	StartOn();
}

void UFlashingPointLightComponent::SetFlashingPattern(TArray<float> Pattern)
{
	FlashingPattern = Pattern;

	StartOn();
}

void UFlashingPointLightComponent::SetFlashingPattern(float FlashTime)
{
	FlashingPattern.Empty();
	FlashingPattern.Add(FlashTime);
}

void UFlashingPointLightComponent::StartOn()
{
	SetVisibility(true);

	if (FlashingPattern.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ToggleLight, this, &UFlashingPointLightComponent::ToggleLight, FlashingPattern[0], false);
	}
}

void UFlashingPointLightComponent::StopOff()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ToggleLight);
	SetVisibility(false);
}

void UFlashingPointLightComponent::ToggleLight()
{
	ToggleVisibility();

	if (FlashingPattern.Num() > 0)
	{
		CurrentPatternIndex = (CurrentPatternIndex + 1) % FlashingPattern.Num();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ToggleLight, this, &UFlashingPointLightComponent::ToggleLight, FlashingPattern[CurrentPatternIndex], false);
	
}
