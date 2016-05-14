// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalPlayerState.h"
#include "Survival/Game/SurvivalGameState.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
	TeamIdx = 0;
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

void ASurvivalPlayerState::OnRep_TeamIdx()
{
	AssignToTeam(TeamIdx);
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalPlayerState, TeamIdx);
}

