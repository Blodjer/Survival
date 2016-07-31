// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalLocalPlayer.h"
#include "Persistent/SurvivalUserSettings.h"

USurvivalUserSettings* USurvivalLocalPlayer::GetSurvivalUserSettings()
{
	if (SurvivalUserSettings == nullptr)
	{
		SurvivalUserSettings = USurvivalUserSettings::LoadSettings();
	}
	return SurvivalUserSettings;
}
