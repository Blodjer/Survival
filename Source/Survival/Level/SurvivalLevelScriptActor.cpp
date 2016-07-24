// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalLevelScriptActor.h"

void ASurvivalLevelScriptActor::HandleMatchHasStarted()
{
	for (ABlockingVolume* BlockingVolume : PreMatchBlockingVolumes)
	{
		if (BlockingVolume != nullptr)
		{
			BlockingVolume->Destroy(true);
		}
	}
}
