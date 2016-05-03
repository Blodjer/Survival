// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Campfire.h"

ACampfire::ACampfire()
{
	CaptureSphere = CreateDefaultSubobject<USphereComponent>("CaptureSphere");
	CaptureSphere->InitSphereRadius(500.0f);
	RootComponent = CaptureSphere;

	SmokeParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("Smoke");
	SmokeParticleSystem->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void ACampfire::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACampfire::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

