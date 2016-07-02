// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalFunctionLibrary.h"

int32 USurvivalFunctionLibrary::GetGlobalSeed(const UObject* Object)
{
	UWorld* World = GEngine->GetWorldFromContextObject(Object);
	if (World && World->GetGameState())
	{
		ASurvivalGameState* SurvivalGameState = Cast<ASurvivalGameState>(World->GetGameState());
		if (SurvivalGameState)
		{
			return SurvivalGameState->GetReplicatedSeed();
		}
	}

	return FMath::Rand();
}
