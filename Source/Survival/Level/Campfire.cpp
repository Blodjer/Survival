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

	OwningTeamIdx = -1;
	DominantTeamIdx = -1;

	CaptureDuration = 10.0f;

	CaptureValue = 0.0f;

	SmokeBaseColor = FLinearColor(0.15f, 0.15f, 0.15f);

	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostPhysics;
}

void ACampfire::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SmokeParticleSystem->SetColorParameter("SmokeColor", SmokeBaseColor);
}

void ACampfire::BeginPlay()
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
				SurvivalGameMode->RegisterCampfire(this);
			}
		}
	}
}

void ACampfire::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	if (DominantTeamIdx != -1)
	{
		if (DominantTeamIdx != OwningTeamIdx)
		{
			CaptureValue = FMath::Clamp(CaptureValue - DeltaTime / CaptureDuration, 0.0f, 1.0f);

			if (CaptureValue <= 0.0f)
			{
				OwningTeamIdx = DominantTeamIdx;
				ASurvivalGameState* SurvialGameState = Cast<ASurvivalGameState>(GetWorld()->GetGameState());
				if (SurvialGameState)
				{
					OwnerBaseColor = SurvialGameState->GetTeamInfo(OwningTeamIdx).Color;
				}
			}
		}
		else if (CaptureValue < 1.0f)
		{
			CaptureValue = FMath::Clamp(CaptureValue + DeltaTime / CaptureDuration, 0.0f, 1.0f);
		}
	}

	FLinearColor SmokeColor = FMath::Lerp(SmokeBaseColor, OwnerBaseColor, FMath::Pow(CaptureValue, 4));
	SmokeParticleSystem->SetColorParameter("SmokeColor", SmokeColor);
}

bool ACampfire::IsCaptured()
{
	return CaptureValue >= 1.0f;
}

void ACampfire::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(OtherActor);
	if (SurvivalPlayerCharacter)
	{
		SurvivalPlayerCharacter->CapturingCampfire = this;

		if (!HasAuthority())
			return;

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
		SurvivalPlayerCharacter->CapturingCampfire = nullptr;

		if (!HasAuthority())
			return;

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
			this->DominantTeamIdx = DominantTeamIdx;
		}
		else
		{
			this->DominantTeamIdx = -1;
		}
	}
	else
	{
		this->DominantTeamIdx = -1;
	}
}

void ACampfire::OnRep_OwningTeam()
{
	if (OwningTeamIdx >= 0)
	{
		ASurvivalGameState* SurvialGameState = Cast<ASurvivalGameState>(GetWorld()->GetGameState());
		if (SurvialGameState)
		{
			OwnerBaseColor = SurvialGameState->GetTeamInfo(OwningTeamIdx).Color;
		}
	}
}

void ACampfire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACampfire, CaptureValue);
	DOREPLIFETIME(ACampfire, OwningTeamIdx);
	DOREPLIFETIME(ACampfire, DominantTeamIdx);
}
