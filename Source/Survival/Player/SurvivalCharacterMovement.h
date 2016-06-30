// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SurvivalCharacterMovement.generated.h"

USTRUCT(BlueprintType)
struct SURVIVAL_API FCharacterMovementValues : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float StandingIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float StandingMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float CrouchingIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	float CrouchingMove;

	float GetValue(ACharacter* Character) const
	{
		if (Character == nullptr)
			return StandingIdle;

		float value = 0.0f;

		bool bIsMoving = !Character->GetVelocity().IsNearlyZero(0.5f);
		if (Character->bIsCrouched)
			value = bIsMoving ? CrouchingMove : CrouchingIdle;
		else
			value = bIsMoving ? StandingMove : StandingIdle;

		return value;
	}
};

UCLASS()
class SURVIVAL_API USurvivalCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USurvivalCharacterMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

	// Return true if we have a valid SurvivalCharacterOwner
	virtual bool HasValidData() const;

	// Return Maximum speed of component in current movement state
	virtual float GetMaxSpeed() const override;

public:
	// The maximum ground speed when sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedRunning;

	// If true the player wants to sprint
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character Movement (General Settings)")
	bool bWantsToSprint;

protected:
	// Character movement component belongs to 
	UPROPERTY()
	class ASurvivalPlayerCharacter* SurvivalCharacterOwner;

};
