// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "AirdropLandingZone.h"
#include "Game/SurvivalGameMode.h"

AAirdropLandingZone::AAirdropLandingZone()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;
	
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	Billboard->SetRelativeLocation(FVector(0, 0, 60.0f));
	Billboard->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/Waypoint.Waypoint")));
	Billboard->SetupAttachment(RootComponent);

	bNetLoadOnClient = false;

	PrimaryActorTick.bCanEverTick = false;
}

void AAirdropLandingZone::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && GetWorld())
	{
		AGameMode* GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode)
		{
			ASurvivalGameMode* SurvivalGameMode = Cast<ASurvivalGameMode>(GameMode);
			if (SurvivalGameMode)
			{
				SurvivalGameMode->RegisterAirdropLandingZone(this);
			}
		}
	}
}
