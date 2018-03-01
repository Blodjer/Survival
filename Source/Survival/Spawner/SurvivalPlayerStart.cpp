// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerStart.h"

ASurvivalPlayerStart::ASurvivalPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(45.0f, 100.0f);

	TeamIdx = 0;
}
