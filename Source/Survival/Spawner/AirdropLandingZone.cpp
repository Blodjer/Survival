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
	ApproachAngle = 55.0f;

	bIsCampLandingZone = false;

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
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
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
	
	if (GetWorld() && GetWorld()->bBegunPlay)
		return;

	DrawDebugCircle(GetWorld(), GetActorLocation(), Radius, 64, FColor::Magenta, false, -1, 0, 25, FVector(0,1,0), FVector(1,0,0), false);

	float Angle = FMath::Min(ApproachAngle, 89.0f);
	float ConeOffset = FMath::Tan(FMath::DegreesToRadians(Angle)) * Radius;
	DrawDebugCone(GetWorld(), GetActorLocation() - FVector(0, 0, ConeOffset), FVector(0, 0, 1), 3750.0f + ConeOffset + FMath::Sqrt(Radius * Radius + ConeOffset + ConeOffset), FMath::DegreesToRadians(90.0f - Angle), FMath::DegreesToRadians(90.0f - Angle), 12, FColor::Magenta, false, -1, 0, 7);
}

bool AAirdropLandingZone::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

void AAirdropLandingZone::GetRandomApproachLocation(float DropHeight, FVector& StartLocation, FVector& LandingLocation) const
{
	LandingLocation = GetActorLocation() + FVector(FMath::FRand() * Radius, 0, 0).RotateAngleAxis(FMath::FRand() * 360.0f, FVector::UpVector);
	StartLocation = LandingLocation + FRotator(FMath::Clamp(ApproachAngle, 0.0f, 90.0f), FMath::FRand() * 360.0f, 0.0f).RotateVector(FVector(DropHeight, 0, 0));
}
