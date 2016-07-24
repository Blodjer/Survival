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

	DefaultPlayerName = FText::FromString("Survivor");

	InactivePlayerStateLifeSpan = 150.0f;

	Teams.Add(FTeamInfo("Alpha", FColor::Blue));
	Teams.Add(FTeamInfo("Bravo", FColor::Red));

	AirdropInterval = FFloatSpan(60.0f, 120.0f);

	ActiveCampfireCluster = -1;
}

void ASurvivalGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Teams.Num() < 1)
	{
		Teams.Add(FTeamInfo());
	}

	DetermineActiveCampfireCluster();
}

void ASurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	MaxPlayers = UGameplayStatics::GetIntOption(Options, FString("MaxPlayers"), -1);
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
			SurvivalPlayerState->AssignToTeam(ChooseTeam(SurvivalPlayerState));
		}
	}

	Super::PostLogin(NewPlayer);
}

int32 ASurvivalGameMode::ChooseTeam(APlayerState* PlayerState)
{
	TArray<int32> PlayersInTeam;
	PlayersInTeam.AddZeroed(Teams.Num());

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ASurvivalPlayerController* SurvivalPlayerController = Cast<ASurvivalPlayerController>(Iterator->Get());
		if (SurvivalPlayerController)
		{
			if (SurvivalPlayerController->PlayerState == PlayerState)
				continue;

			ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(SurvivalPlayerController->PlayerState);
			if (SurvivalPlayerState)
			{
				PlayersInTeam[SurvivalPlayerState->GetTeamIdx()]++;
			}
		}
	}

	int32 TeamIdx = 0;
	for (int32 i = 0; i < PlayersInTeam.Num(); i++)
	{
		if (PlayersInTeam[i] < PlayersInTeam[TeamIdx])
		{
			TeamIdx = i;
		}
	}

	return TeamIdx;
}

int32 ASurvivalGameMode::ChooseTeam(APlayerState* PlayerState, int32 PreferedTeam)
{
	if (PreferedTeam >= 0 && PreferedTeam < Teams.Num() && MaxPlayers > 0)
	{
		TArray<int32> PlayersInTeam;
		PlayersInTeam.AddZeroed(Teams.Num());

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ASurvivalPlayerController* SurvivalPlayerController = Cast<ASurvivalPlayerController>(Iterator->Get());
			if (SurvivalPlayerController)
			{
				if (SurvivalPlayerController->PlayerState == PlayerState)
					continue;

				ASurvivalPlayerState* SurvivalPlayerState = Cast<ASurvivalPlayerState>(SurvivalPlayerController->PlayerState);
				if (SurvivalPlayerState)
				{
					PlayersInTeam[SurvivalPlayerState->GetTeamIdx()]++;
				}
			}
		}

		int32 MaxPlayersPerTeam = MaxPlayers / Teams.Num();
		if (PlayersInTeam[PreferedTeam] < MaxPlayersPerTeam)
		{
			return PreferedTeam;
		}
	}

	return ChooseTeam(PlayerState);
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

	if (IsMatchInProgress())
	{
		int32 WinnerTeamIdx;
		if (DetermineMatchWinner(WinnerTeamIdx))
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
}

void ASurvivalGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	
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

void ASurvivalGameMode::DetermineActiveCampfireCluster()
{
	ActiveCampfires.Empty();

	if (Campfires.Num() == 0)
		return;

	TArray<int32> ClusterIdxs;
	for (ACampfire* Campfire : Campfires)
	{
		if (Campfire->GetClusterIdx() != -1)
		{
			ClusterIdxs.Add(Campfire->GetClusterIdx());
		}
	}

	if (ClusterIdxs.Num() <= 1)
		return;

	ActiveCampfireCluster = ClusterIdxs[FMath::RandRange(0, ClusterIdxs.Num() - 1)];
	for (ACampfire* Campfire : Campfires)
	{
		if (Campfire->GetClusterIdx() != ActiveCampfireCluster && Campfire->GetClusterIdx() != -1)
		{
			Campfire->SetActive(false);
		}
		else
		{
			Campfire->SetActive(true);
			ActiveCampfires.Add(Campfire);
		}
	}
}

bool ASurvivalGameMode::DetermineMatchWinner(int32& WinnerTeamIdx)
{
	// Captured all Campfires
	for (int32 TeamIdx = 0; TeamIdx < Teams.Num(); TeamIdx++)
	{
		int32 OwnedCampfires = 0;
		for (ACampfire* Campfire : ActiveCampfires)
		{
			if (Campfire->IsCaptured() && Campfire->GetOwningTeamIdx() == TeamIdx)
			{
				OwnedCampfires++;

				if (OwnedCampfires == ActiveCampfires.Num())
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

	TSubclassOf<AAirdropSupplyBox> SupplyClass = nullptr;
	AAirdropLandingZone* LandingZoneActor = nullptr;

	float TimeInMinutes = GetGameState<ASurvivalGameState>() ? GetGameState<ASurvivalGameState>()->GetMatchTime() / 60.0f : 0.0f;

	float TotalProbability = 0.0f;
	TArray<float> Probabilities;
	for (FAirdropSupply Supply : AirdropSupplies)
	{
		float Probability = Supply.GetProbability(TimeInMinutes);
		if (SuppliesProbabilityModifier.Contains(Supply.SupplyClass))
		{
			Probability += Probability * 0.4f * SuppliesProbabilityModifier[Supply.SupplyClass];
		}

		TotalProbability += Probability;
		Probabilities.Add(Probability);
	}

	float SupplyProbabilityIndex = FMath::RandRange(0.0f, TotalProbability);

	float CurrentProbabilityIndex = 0.0f;
	for (int32 i = 0; i < Probabilities.Num(); i++)
	{
		CurrentProbabilityIndex += Probabilities[i];
		if (CurrentProbabilityIndex >= SupplyProbabilityIndex && SupplyClass == nullptr)
		{
			SupplyClass = AirdropSupplies[i].SupplyClass;
			SuppliesProbabilityModifier.FindOrAdd(SupplyClass) = 1;
		}
		else
		{
			int& p = SuppliesProbabilityModifier.FindOrAdd(AirdropSupplies[i].SupplyClass);
			p = FMath::Max(1, p) + 1;
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
	
	FVector StartLocation, LandingLocation;
	LandingZone->GetRandomApproachLocation(11000.0f, StartLocation, LandingLocation);
	
	const FTransform SpawnTransform(StartLocation);

	AAirdrop* Airdrop = GetWorld()->SpawnActorDeferred<AAirdrop>(AirdropClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Airdrop)
	{
		Airdrop->Init(LandingLocation, Payload);
		UGameplayStatics::FinishSpawningActor(Airdrop, SpawnTransform);
	}
}
