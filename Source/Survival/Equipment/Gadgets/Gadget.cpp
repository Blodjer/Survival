// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Gadget.h"

AGadget::AGadget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Type = EHandheldType::Gadget;
}

void AGadget::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("GadgetPrimaryAction", EInputEvent::IE_Pressed, this, &AGadget::Use);
}

void AGadget::Use()
{
	if (!IsGameInputAllowed())
		return;

	ServerUse();
}

void AGadget::ServerUse_Implementation()
{
	OnUse();

	SimulateUse();

	if (bIsDisposable && GetOwnerCharacter() != nullptr)
	{
		Drop(true);
	}
}

void AGadget::OnUse_Implementation()
{

}

void AGadget::SimulateUse_Implementation()
{
	// TODO: Play Animation
}
