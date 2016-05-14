// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Campfire.h"

ACampfire::ACampfire()
{
	CaptureSphere = CreateDefaultSubobject<USphereComponent>("CaptureSphere");
	CaptureSphere->InitSphereRadius(500.0f);
	CaptureSphere->OnComponentBeginOverlap.AddDynamic(this, &ACampfire::OnBeginOverlap);
	CaptureSphere->OnComponentEndOverlap.AddDynamic(this, &ACampfire::OnEndOverlap);
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

void ACampfire::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(OtherActor);
	if (SurvivalPlayerCharacter)
	{
		if (SurvivalPlayerCharacter->PlayerState)
		{
			CapturingPlayers.Add(SurvivalPlayerCharacter);
			OnCapturingPlayersChanged();
		}
	}
}

void ACampfire::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(OtherActor);
	if (SurvivalPlayerCharacter)
	{
		CapturingPlayers.Remove(SurvivalPlayerCharacter);
		OnCapturingPlayersChanged();
	}
}

void ACampfire::OnCapturingPlayersChanged()
{
	TMap<int32, uint32> CapturingTeams;
	for (ASurvivalPlayerCharacter* SurvialPlayerCharacter : CapturingPlayers)
	{
		int32 TeamIdx = SurvialPlayerCharacter->GetTeamIdx();
		if (CapturingTeams.Contains(TeamIdx))
		{
			CapturingTeams[TeamIdx]++;
		}
		else
		{
			CapturingTeams.Add(TeamIdx, 1);
		}
	}

	if (CapturingTeams.Num() > 0)
	{
		TArray<int32, FDefaultAllocator> CapturingTeamsKeys;
		CapturingTeams.GenerateKeyArray(CapturingTeamsKeys);

		int32 DominantTeamIdx = CapturingTeamsKeys[0];
		bool bDraw = false;

		for (auto& Element : CapturingTeams)
		{
			if (Element.Value > CapturingTeams[DominantTeamIdx])
			{
				DominantTeamIdx = Element.Key;
				bDraw = false;
			}
			else if (Element.Key != DominantTeamIdx && Element.Value == CapturingTeams[DominantTeamIdx])
			{
				bDraw = true;
			}
		}

		if (!bDraw)
		{
			ASurvivalGameState* SurvialGameState = Cast<ASurvivalGameState>(GetWorld()->GetGameState());
			if (SurvialGameState)
			{
				OwningTeam = SurvialGameState->GetTeamInfo(DominantTeamIdx);
				SmokeParticleSystem->SetColorParameter("SmokeColor", OwningTeam.Color);
			}
		}
	}
}
