// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Character.h"
#include "SurvivalPlayerCharacter.generated.h"

UCLASS()
class SURVIVAL_API ASurvivalPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivalPlayerCharacter();

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void TurnAtRate(float Value);

	void LookUpAtRate(float Value);

public:
	// First person mesh. Seen only by owner
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	class USkeletalMeshComponent* Mesh1P;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// Base controller turn rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	// Base controller look up/down rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
};
