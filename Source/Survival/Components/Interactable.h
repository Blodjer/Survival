// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Interactable.generated.h"

UINTERFACE(Blueprintable)
class SURVIVAL_API UInteractable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class SURVIVAL_API IInteractable
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = Interactable)
	void Interact(class ASurvivalPlayerCharacter* PlayerCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
	bool IsInteractable(ASurvivalPlayerCharacter* PlayerCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
	bool IsRenderedAsInteractable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
	FName GetInteractableName() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Interactable)
	FString GetInteractionVerb() const;

};
