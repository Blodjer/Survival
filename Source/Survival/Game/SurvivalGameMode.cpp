// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameMode.h"
#include "SurvivalGameState.h"
#include "Survival/Spawner/SurvivalPlayerStart.h"
#include "Survival/Player/SurvivalPlayerState.h"
#include "Survival/Level/Campfire.h"
#include "Survival/Level/Airdrop.h"
#include "Survival/Spawner/AirdropLandingZone.h"

ASurvivalGameMode::ASurvivalGameMode()
{
	LengthOfDay = 60.0f;
	StartTimeOfDay = 8.5f;

	MinRespawnDelay = 10.0f;
	MinDieDelay = 10.0f;

	Teams.Add(FTeamInfo("Alpha", FColor::Blue));
	Teams.Add(FTeamInfo("Bravo", FColor::Red));

	AirdropInterval = FFloatSpan(60.0f, 120.0f);
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
			if (SurvivalPlayerStart->GetTeamIdx() == SurvivalPlayerState->GetTeamIdx())
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

void ASurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int32 WinnerTeamIdx;
	if (IsMatchInProgress() && DetermineMatchWinner(WinnerTeamIdx))
	{
		EndMatch();

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ASurvivalPlayerController* SurvivalPlayerController = Cast<ASurvivalPlayerController>(Iterator->Get());
			if (SurvivalPlayerController)
			{
				SurvivalPlayerController->MatchHasEnded(WinnerTeamIdx);
			}
		}
	}
}

void ASurvivalGameMode::StartMatch()
{
	Super::StartMatch();

	GetWorldTimerManager().SetTimer(TimerHandle_SendAirdrop, this, &ASurvivalGameMode::DetermineNextAirdrop, AirdropInterval.Random(), false);
}

void ASurvivalGameMode::Killed(const UDamageType* DamageType, AController* Killer, AController* KilledPlayer)
{
	if (Killer != nullptr && Killer->PlayerState != nullptr && KilledPlayer != nullptr && KilledPlayer->PlayerState != nullptr)
	{
		FString KillerName = Killer->PlayerState->GetHumanReadableName();
		FString KilledPlayerName = KilledPlayer->PlayerState->GetHumanReadableName();

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString(KillerName + " killed " + KilledPlayerName));
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DamageType->bCausedByWorld ? "World killed you" : "unknown killer");
	}
}

void ASurvivalGameMode::RegisterCampfire(ACampfire* Campfire)
{
	Campfires.Add(Campfire);
}

void ASurvivalGameMode::RegisterAirdropLandingZone(AAirdropLandingZone* LandingZone)
{
	AirdropLandingZones.Add(LandingZone);
}

bool ASurvivalGameMode::DetermineMatchWinner(int32& WinnerTeamIdx)
{
	// Captured all Campfires
	for (int32 TeamIdx = 0; TeamIdx < Teams.Num(); TeamIdx++)
	{
		int32 OwnedCampfires = 0;
		for (ACampfire* Campfire : Campfires)
		{
			if (Campfire->IsCaptured() && Campfire->GetOwningTeamIdx() == TeamIdx)
			{
				OwnedCampfires++;

				if (OwnedCampfires == Campfires.Num())
				{
					WinnerTeamIdx = TeamIdx;
					return true;
				}
			}
		}
	}

	// Killed all other teams
	if (Teams.Num() >= 2)
	{
		TMap<int32, uint32> AlivePlayersInTeam;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ASurvivalPlayerController* SurvivalPlayerController = Cast<ASurvivalPlayerController>(Iterator->Get());
			if (SurvivalPlayerController)
			{
				ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(SurvivalPlayerController->PlayerState);
				if (SurvivalPlayerState)
				{
					uint32& PlayersAlive = AlivePlayersInTeam.FindOrAdd(SurvivalPlayerState->GetTeamIdx());

					if (!(SurvivalPlayerController->IsFrozen() || SurvivalPlayerController->GetPawn() && SurvivalPlayerController->GetPawn()->IsPendingKill()))
					{
						PlayersAlive++;
					}				
				}
			}
		}
		if (AlivePlayersInTeam.Num() >= 2)
		{
			for (int32 TeamIdx = 0; TeamIdx < Teams.Num(); TeamIdx++)
			{
				uint32* PlayersAlive = AlivePlayersInTeam.Find(TeamIdx);
				if (PlayersAlive != nullptr)
				{
					if (*PlayersAlive == 0)
					{
						AlivePlayersInTeam.Remove(TeamIdx);
					}
				}
			}
			if (AlivePlayersInTeam.Num() == 1)
			{
				TArray<int32> WinnerTeams;
				AlivePlayersInTeam.GetKeys(WinnerTeams);
				WinnerTeamIdx = WinnerTeams[0];
				return true;
			}
		}
	}

	return false;
}

void ASurvivalGameMode::DetermineNextAirdrop()
{
	if (AirdropSupplies.Num() == 0 || AirdropLandingZones.Num() == 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SendAirdrop, this, &ASurvivalGameMode::DetermineNextAirdrop, AirdropInterval.Random(), false);
		return;
	}

	TSubclassOf<AAirdropSupplyBox> SupplyClass;
	AAirdropLandingZone* LandingZoneActor = nullptr;

	float TimeInMinutes = GetGameState<ASurvivalGameState>() ? GetGameState<ASurvivalGameState>()->GetMatchTime() / 60.0f : 0.0f;

	float TotalProbability = 0.0f;
	for (FAirdropSupply Supply : AirdropSupplies)
	{
		TotalProbability += Supply.GetProbability(TimeInMinutes);
	}

	float SupplyProbabilityIndex = FMath::RandRange(0.0f, TotalProbability);

	float i = 0.0f;
	for (FAirdropSupply Supply : AirdropSupplies)
	{
		i += Supply.GetProbability(TimeInMinutes);
		if (i >= SupplyProbabilityIndex)
		{
			SupplyClass = Supply.SupplyClass;
			break;
		}
	}

	TArray<AAirdropLandingZone*> QualifiedLandingZones;
	for (AAirdropLandingZone* LandingZone : AirdropLandingZones)
	{
		if (LandingZone->GetAirDropSupplies().Contains(SupplyClass))
		{
			QualifiedLandingZones.Add(LandingZone);
		}
	}

	if (QualifiedLandingZones.Num() > 0)
	{
		LandingZoneActor = QualifiedLandingZones[FMath::RandRange(0, QualifiedLandingZones.Num() - 1)];
	}

	SendAirdrop(SupplyClass, LandingZoneActor);

	GetWorldTimerManager().SetTimer(TimerHandle_SendAirdrop, this, &ASurvivalGameMode::DetermineNextAirdrop, AirdropInterval.Random(), false);
}

void ASurvivalGameMode::SendAirdrop(TSubclassOf<AAirdropSupplyBox> Payload, AAirdropLandingZone* LandingZone)
{
	if (Payload == nullptr || LandingZone == nullptr)
		return;

	const FVector SpawnOffset(0.0f, 4500.0f, 10000.0f);
	const FTransform SpawnTransform(LandingZone->GetActorLocation() + SpawnOffset);

	AAirdrop* Airdrop = GetWorld()->SpawnActorDeferred<AAirdrop>(AirdropClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Airdrop)
	{
		Airdrop->Init(LandingZone->GetActorLocation(), Payload);
		UGameplayStatics::FinishSpawningActor(Airdrop, SpawnTransform);
	}
}
