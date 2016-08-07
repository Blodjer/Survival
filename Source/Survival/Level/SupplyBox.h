// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Survival/Components/DroppablePhysicsActor.h"
#include "SupplyBox.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API ASupplyBox : public ADroppablePhysicsActor, public IInteractable
{
	GENERATED_BODY()

public:
	ASupplyBox();

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE FString GetInteractionVerb_Implementation() const override { return "Open"; };

	FORCEINLINE FName GetInteractableName_Implementation() const override { return "Supplies"; };

	virtual bool IsInteractable_Implementation(class ASurvivalPlayerCharacter* PlayerCharacter) override;

	virtual bool IsRenderedAsInteractable_Implementation() override;

	virtual void Interact_Implementation(class ASurvivalPlayerCharacter* PlayerCharacter) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = SupplyBox)
	virtual void Open();

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsOpen, Category = SupplyBox)
	bool bIsOpen;

	UPROPERTY(BlueprintReadOnly, Category = SupplyBox)
	float OpenTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SupplyBox)
	UCurveFloat* CoverOpenRotationCurve;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = SupplyBox, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CaseMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = SupplyBox, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoverMesh;

private:
	UFUNCTION()
	void OnRep_IsOpen();

public:
	FORCEINLINE UStaticMeshComponent* GetCaseMesh() const { return CaseMesh; };

};
