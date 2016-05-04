// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameMode.h"
#include "SurvivalGameState.h"
#include "Survival/Spawner/SurvivalPlayerStart.h"
#include "Survival/Player/SurvivalPlayerState.h"


ASurvivalGameMode::ASurvivalGameMode()
{
	LengthOfDay = 60.0f;
	StartTimeOfDay = 8.5f;

	Teams.Add(FTeamInfo("Alpha", FColor::Blue));
	Teams.Add(FTeamInfo("Beta", FColor::Red));
}

void ASurvivalGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Teams.Num() < 1)
	{
		Teams.Add(FTeamInfo());
	}
}

void ASurvivalGameMode::InitGameState()
{
	Super::InitGameState();

	ASurvivalGameState* SurvivalGameState = GetGameState<ASurvivalGameState>();
	if (SurvivalGameState)
	{
		SurvivalGameState->SetMatchProperties(LengthOfDay, StartTimeOfDay, Teams);
	}
}

void ASurvivalGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (NewPlayer->PlayerState)
	{
		ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(NewPlayer->PlayerState);
		if (SurvivalPlayerState)
		{
			SurvivalPlayerState->AssignToTeam(GetNumPlayers() % Teams.Num());
		}
	}

	Super::PostLogin(NewPlayer);
}

AActor* ASurvivalGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerStart* FoundPlayerStart = nullptr;

	TArray<APlayerStart*> UnOccupiedTeamStartPoints;
	TArray<APlayerStart*> OccupiedTeamStartPoints;
	TArray<APlayerStart*> NeutralStartPoints;

	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			FoundPlayerStart = PlayerStart;
			break;
		}
		else if (PlayerStart->IsA<ASurvivalPlayerStart>() && Player->PlayerState && Player->PlayerState->IsA<ASurvivalPlayerState>())
		{
			ASurvivalPlayerStart* SurvivalPlayerStart = Cast<ASurvivalPlayerStart>(PlayerStart);
			ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(Player->PlayerState);
			if (SurvivalPlayerStart->GetTeamNumber() == SurvivalPlayerState->GetTeamNumber())
			{
				FVector ActorLocation = PlayerStart->GetActorLocation();
				FRotator ActorRotation = PlayerStart->GetActorRotation();
				if (!GetWorld()->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
				{
					UnOccupiedTeamStartPoints.Add(PlayerStart);
				}
				else if (GetWorld()->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
				{
					OccupiedTeamStartPoints.Add(PlayerStart);
				}
			}
		}
		else
		{
			NeutralStartPoints.Add(PlayerStart);
		}
	}

	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedTeamStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedTeamStartPoints[FMath::RandRange(0, UnOccupiedTeamStartPoints.Num() - 1)];
		}
		else if (OccupiedTeamStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedTeamStartPoints[FMath::RandRange(0, OccupiedTeamStartPoints.Num() - 1)];
		}
		else if (NeutralStartPoints.Num() > 0)
		{
			FoundPlayerStart = NeutralStartPoints[FMath::RandRange(0, NeutralStartPoints.Num() - 1)];
		}
	}

	return FoundPlayerStart ? FoundPlayerStart : Super::ChoosePlayerStart_Implementation(Player);
}

bool ASurvivalGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void ASurvivalGameMode::Killed(const UDamageType* DamageType, AController* Killer, AController* KilledPlayer)
{
	if (Killer != nullptr && Killer->PlayerState != nullptr && KilledPlayer != nullptr && KilledPlayer->PlayerState != nullptr)
	{
		FString KillerName = Killer->PlayerState->GetHumanReadableName();
		FString KilledPlayerName = KilledPlayer->PlayerState->GetHumanReadableName();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString(KillerName + " killed " + KilledPlayerName));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DamageType->bCausedByWorld ? "World killed you" : "unknown killer");
	}
}
