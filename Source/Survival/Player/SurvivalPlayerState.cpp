// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerState.h"
#include "Survival/Game/SurvivalGameState.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
	TeamNumber = 0;
}

void ASurvivalPlayerState::AssignToTeam(int32 Number)
{
	TeamNumber = Number;

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

FTeamInfo ASurvivalPlayerState::GetTeamInfo()
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		ASurvivalGameState* SurvivalGameState = Cast<ASurvivalGameState>(GetWorld()->GetGameState());
		if (SurvivalGameState)
		{
			if (TeamNumber >= 0 && SurvivalGameState->GetNumberOfTeams() >= TeamNumber + 1)
			{
				return SurvivalGameState->GetTeamInfos()[TeamNumber];
			}
		}
	}

	return FTeamInfo();
}

void ASurvivalPlayerState::OnRep_TeamNumber()
{
	AssignToTeam(TeamNumber);
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalPlayerState, TeamNumber);
}

