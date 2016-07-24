// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerState.h"
#include "Survival/Game/SurvivalGameState.h"
#include "Survival/Game/SurvivalGameMode.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
	TeamIdx = 0;
}

void ASurvivalPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	ASurvivalPlayerState* NewSurvivalPlayerState = Cast<ASurvivalPlayerState>(NewPlayerState);
	if (NewSurvivalPlayerState)
	{
		if (GetWorld() && GetWorld()->GetAuthGameMode())
		{
			ASurvivalGameMode* SurvivalGameMode = Cast<ASurvivalGameMode>(GetWorld()->GetAuthGameMode());
			if (SurvivalGameMode)
			{
				NewSurvivalPlayerState->AssignToTeam(SurvivalGameMode->ChooseTeam(this, TeamIdx));
			}
		}
	}
}

void ASurvivalPlayerState::AssignToTeam(int32 Number)
{
	TeamIdx = Number;

	AController* Controller = Cast<AController>(GetOwner());
	if (Controller)
	{
		ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(Controller->GetCharacter());
		if (SurvivalPlayerCharacter)
		{
			SurvivalPlayerCharacter->OnUpdateTeamInfo();
		}
	}
}

bool ASurvivalPlayerState::GetTeamInfo(FTeamInfo& TeamInfo)
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		ASurvivalGameState* SurvivalGameState = Cast<ASurvivalGameState>(GetWorld()->GetGameState());
		if (SurvivalGameState)
		{
			if (TeamIdx >= 0 && SurvivalGameState->GetNumberOfTeams() >= TeamIdx + 1)
			{
				TeamInfo = SurvivalGameState->GetTeamInfos()[TeamIdx];
				return true;
			}
		}
	}
	
	return false;
}

TArray<ASurvivalPlayerCharacter*> ASurvivalPlayerState::GetSpawnedTeamMates()
{
	TArray<ASurvivalPlayerCharacter*> TeamMates;
	for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		ASurvivalPlayerCharacter* SurvivalPlayerCharacter = Cast<ASurvivalPlayerCharacter>(Iterator->Get());
		if (SurvivalPlayerCharacter)
		{
			if (!SurvivalPlayerCharacter->IsLocallyControlled() && SurvivalPlayerCharacter->GetTeamIdx() == GetTeamIdx())
			{
				TeamMates.Add(SurvivalPlayerCharacter);
			}
		}
	}

	return TeamMates;
}

void ASurvivalPlayerState::OnRep_TeamIdx()
{
	AssignToTeam(TeamIdx);
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalPlayerState, TeamIdx);
}

