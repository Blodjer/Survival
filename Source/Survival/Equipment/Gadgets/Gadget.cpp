// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Gadget.h"

AGadget::AGadget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Type = EHandheldType::Gadget;

	UseDelay = 1.0f;
	Cooldown = 1.0f;

	bIsInUse = false;
}

void AGadget::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("GadgetPrimaryAction", EInputEvent::IE_Pressed, this, &AGadget::Use);
}

void AGadget::OnCharacterStopUse()
{
	Super::OnCharacterStopUse();

	StopAnimation(UseAnimation);
}

void AGadget::Use()
{
	if (!IsGameInputAllowed())
		return;

	ServerUse();
}

void AGadget::ServerUse_Implementation()
{
	if (bIsInUse)
		return;

	SimulateUse();

	bIsInUse = true;

	GetWorldTimerManager().SetTimer(TimerHandle_UseDelay, this, &AGadget::OnUse, UseDelay);
	GetWorldTimerManager().SetTimer(TimerHandle_Cooldown, this, &AGadget::CooldownEnd, Cooldown);
}

void AGadget::OnUse_Implementation()
{
	if (bIsDisposable && GetOwnerCharacter() != nullptr)
	{
		Drop(true);
	}
}

void AGadget::SimulateUse_Implementation()
{
	PlayAnimation(UseAnimation);
}

void AGadget::CooldownEnd()
{
	bIsInUse = false;
}

void AGadget::BeforeDrop()
{
	Super::BeforeDrop();

	StopAnimation(UseAnimation);
}
