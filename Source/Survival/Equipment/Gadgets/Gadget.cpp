// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Gadget.h"

void AGadget::SetupInputActions()
{
	Super::SetupInputActions();

	BindInputAction("GadgetPrimaryAction", EInputEvent::IE_Pressed, this, &AGadget::Use);
}

void AGadget::Use()
{
	ServerUse();
}

void AGadget::ServerUse_Implementation()
{
	OnUse();

	SimulateUse();

	if (bIsDisposable && GetOwnerCharacter() != nullptr)
	{
		GetOwnerCharacter()->RemoveHandheldFromInventory(this);
	}
}

void AGadget::OnUse_Implementation()
{

}

void AGadget::SimulateUse_Implementation()
{
	// TODO: Play Animation
}
