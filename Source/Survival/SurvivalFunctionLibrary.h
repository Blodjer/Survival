// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SurvivalFunctionLibrary.generated.h"

UCLASS()
class SURVIVAL_API USurvivalFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Math|Random")
	static int32 GetGlobalSeed(const UObject* Object);
	
};
