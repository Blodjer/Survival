// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Campfire.h"
#include "Game/SurvivalGameMode.h"

ACampfire::ACampfire()
{
	CaptureSphere = CreateDefaultSubobject<USphereComponent>("CaptureSphere");
	CaptureSphere->InitSphereRadius(500.0f);
	CaptureSphere->OnComponentBeginOverlap.AddDynamic(this, &ACampfire::OnBeginOverlap);
	CaptureSphere->OnComponentEndOverlap.AddDynamic(this, &ACampfire::OnEndOverlap);
	RootComponent = CaptureSphere;

	SmokeParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("Smoke");
	SmokeParticleSystem->SetupAttachment(RootComponent);

	Audio = CreateDefaultSubobject<UAudioComponent>("Audio");
	Audio->SetupAttachment(RootComponent);

	DamageArea = CreateDefaultSubobject<UCapsuleComponent>("DamageArea");
	DamageArea->InitCapsuleSize(80.0f, 200.0f);
	DamageArea->OnComponentBeginOverlap.AddDynamic(this, &ACampfire::OnBeginDamage);
	DamageArea->OnComponentEndOverlap.AddDynamic(this, &ACampfire::OnEndDamage);
	DamageArea->SetCollisionProfileName("OverlapAllDynamic");
	DamageArea->SetupAttachment(RootComponent);

	OwningTeamIdx = -1;
	DominantTeamIdx = -1;

	ClusterIdx = -1;

	bIsActive = true;

	CaptureDuration = 18.0f;
	CaptureDurationMuliplier = 0.5f;

	CaptureValue = 0.0f;
	CurrentCaptureDurationMultiplier = 1.0f;

	SmokeBaseColor = FLinearColor(0.15f, 0.15f, 0.15f);

	FireDamage = 10.0f;

	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostPhysics;
}

void ACampfire::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SmokeParticleSystem->SetColorParameter("SmokeColor", SmokeBaseColor);
}

void ACampfire::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
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
	Super::Tick(DeltaTime);
	
	if (DominantTeamIdx != -1)
	{
		if (DominantTeamIdx != OwningTeamIdx)
		{
			CaptureValue = FMath::Clamp(CaptureValue - DeltaTime / (CaptureDuration * CurrentCaptureDurationMultiplier), 0.0f, 1.0f);

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
			CaptureValue = FMath::Clamp(CaptureValue + DeltaTime / (CaptureDuration * CurrentCaptureDurationMultiplier), 0.0f, 1.0f);
		}
	}

	FLinearColor SmokeColor = FMath::Lerp(SmokeBaseColor, OwnerBaseColor, FMath::Pow(CaptureValue, 4));
	SmokeParticleSystem->SetColorParameter("SmokeColor", SmokeColor);

	for (APawn* Pawn : DamagingPawns)
	{
		if (Pawn)
		{
			Pawn->TakeDamage(FireDamage * DeltaTime, FDamageEvent(), nullptr, this);
		}
	}
}

void ACampfire::SetActive(bool bActive)
{
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
	SetActorTickEnabled(bActive);

	if (bActive)
	{
		Audio->Activate(true);
	}
	else
	{
		Audio->Deactivate();
	}

	bIsActive = bActive;
}

bool ACampfire::IsCaptured()
{
	return CaptureValue >= 0.98f;
}

void ACampfire::UpdateCapturingPlayers()
{
	if (!HasAuthority())
		return;

	CapturingPlayers.Empty();

	TArray<AActor*> OverlappingActors;
	CaptureSphere->GetOverlappingActors(OverlappingActors, ASurvivalPlayerCharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		ASurvivalPlayerCharacter* Player = Cast<ASurvivalPlayerCharacter>(Actor);
		if (Player != nullptr && !Player->IsLethalInjured())
		{
			CapturingPlayers.AddUnique(Player);
		}
	}

	OnCapturingPlayersChanged();
}

void ACampfire::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(OtherActor);
	if (SurvivalPlayerCharacter)
	{
		SurvivalPlayerCharacter->CapturingCampfire = this;

		CapturingPlayers.AddUnique(SurvivalPlayerCharacter);
		OnCapturingPlayersChanged();
	}
}

void ACampfire::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(OtherActor);
	if (SurvivalPlayerCharacter)
	{
		SurvivalPlayerCharacter->CapturingCampfire = nullptr;

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
		CapturingTeams.FindOrAdd(TeamIdx)++;
	}

	if (CapturingTeams.Num() > 0)
	{
		TArray<int32, FDefaultAllocator> CapturingTeamsKeys;
		CapturingTeams.GenerateKeyArray(CapturingTeamsKeys);

		int32 DominantTeamIdx = CapturingTeamsKeys[0];
		bool bDraw = false;

		for (auto& Team : CapturingTeams)
		{
			if (Team.Value > CapturingTeams[DominantTeamIdx])
			{
				DominantTeamIdx = Team.Key;
				bDraw = false;
			}
			else if (Team.Key != DominantTeamIdx && Team.Value == CapturingTeams[DominantTeamIdx])
			{
				bDraw = true;
			}
		}

		if (!bDraw)
		{
			this->DominantTeamIdx = DominantTeamIdx;
			
			int32 TeamPower = CapturingTeams[this->DominantTeamIdx] - (CapturingPlayers.Num() - CapturingTeams[this->DominantTeamIdx]);
			this->CurrentCaptureDurationMultiplier = FMath::Pow(CaptureDurationMuliplier, TeamPower - 1);
		}
		else
		{
			this->DominantTeamIdx = -1;
			this->CurrentCaptureDurationMultiplier = 1.0f;
		}
	}
	else
	{
		this->DominantTeamIdx = -1;
		this->CurrentCaptureDurationMultiplier = 1.0f;
	}
}

void ACampfire::OnBeginDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			DamagingPawns.Add(Pawn);
		}
	}
}

void ACampfire::OnEndDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		DamagingPawns.Remove(Pawn);
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

void ACampfire::OnRep_IsActive()
{
	SetActive(bIsActive);
}

void ACampfire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACampfire, CaptureValue);
	DOREPLIFETIME(ACampfire, OwningTeamIdx);
	DOREPLIFETIME(ACampfire, DominantTeamIdx);
	DOREPLIFETIME(ACampfire, bIsActive);
}
