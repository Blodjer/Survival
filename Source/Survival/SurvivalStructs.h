// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine.h"
#include "SurvivalStructs.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	FLinearColor Color;

	FTeamInfo()
	{
		Name = "Unknown";
		Color = FColor::Magenta;
	}

	FTeamInfo(FName Name, FLinearColor Color)
	{
		this->Name = Name;
		this->Color = Color;
	}
};
