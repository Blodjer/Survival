// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine/LevelScriptActor.h"
#include "SurvivalLevelScriptActor.generated.h"

UCLASS()
class SURVIVAL_API ASurvivalLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	void HandleMatchHasStarted();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PreMatch, meta = (AllowPrivateAccess = "true"))
	TArray<ABlockingVolume*> PreMatchBlockingVolumes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PostMatch, meta = (AllowPrivateAccess = "true"))
	class ALevelSequenceActor* MatchEndSequence;

public:
	FORCEINLINE class ALevelSequenceActor* GetMatchEndSequence() const { return MatchEndSequence; };

};
