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

	Radius = 800.0f;

	bNetLoadOnClient = false;

#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif
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

#if WITH_EDITOR
void AAirdropLandingZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebugCircle(GetWorld(), GetActorLocation(), Radius, 64, FColor::Magenta, false, -1, 0, 25, FVector(0,1,0), FVector(1,0,0), false);
}

bool AAirdropLandingZone::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

FVector AAirdropLandingZone::GetRandomLandingLocation() const
{
	return GetActorLocation() + FVector(FMath::FRand() * Radius, 0, 0).RotateAngleAxis(FMath::FRand() * 360.0f, FVector::UpVector);
}
