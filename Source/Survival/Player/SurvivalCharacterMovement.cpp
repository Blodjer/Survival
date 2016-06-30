// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalCharacterMovement.h"


USurvivalCharacterMovement::USurvivalCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxWalkSpeedRunning = 800.0f;
	bWantsToSprint = false;
}

void USurvivalCharacterMovement::SetUpdatedComponent(USceneComponent * NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
	
	SurvivalCharacterOwner = Cast<ASurvivalPlayerCharacter>(PawnOwner);
}

bool USurvivalCharacterMovement::HasValidData() const
{
	return Super::HasValidData() && IsValid(SurvivalCharacterOwner);
}

float USurvivalCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	
	if (bWantsToSprint)
	{
		if (SurvivalCharacterOwner == nullptr || (Velocity.GetSafeNormal2D() | SurvivalCharacterOwner->GetActorForwardVector()) > -0.1)
		{
			float MaxWalkSpeedRunningOffset = MaxWalkSpeedRunning - MaxWalkSpeed;
			MaxSpeed = FMath::Clamp(MaxWalkSpeed + MaxWalkSpeedRunningOffset * SurvivalCharacterOwner->GetStamina(), MaxWalkSpeed + MaxWalkSpeedRunningOffset * 0.25f, MaxWalkSpeedRunning);
		}
	}

	return MaxSpeed;
}
