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
	
private:
	// Moves the character alongs his x-axis
	void MoveForward(float Value);

	// Moves the character along his y-axis
	void MoveRight(float Value);

	// Turns the character around his z-axis
	void Turn(float Value);

	// Rotates the camera up
	void LookUp(float Value);

	// Turns the character around his z-axis at BaseTurnRate
	void TurnAtRate(float Value);

	// Rotates the camera up at BaseLookUpRate
	void LookUpAtRate(float Value);
	
	// Toggle the light visibility on/off
	void ToggleFlashlight();

	// Set the light on or off. Replicates to server
	void SetFlashlightOn(bool bOn);

	// Set the light on or off on the server. Replicate to all clients except the owner
	UFUNCTION(Server, Reliable, WithValidation, Category = Flashlight)
	void ServerSetFlashlightOn(bool bOn);
	void ServerSetFlashlightOn_Implementation(bool bOn);
	bool ServerSetFlashlightOn_Validate(bool bOn) { return true; };

private:
	// First person mesh. Seen only by owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	
	// Spotlight used as flashlight. Attached to view.
	UPROPERTY(VisibleAnywhere, Category = Flashlight)
	class USpotLightComponent* Flashlight;
	
	// Is the flashlight turned on or off
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsFlashlightOn)
	bool bIsFlashlightOn;

protected:
	// Base controller turn rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	// Base controller look up/down rate, in deg/sec
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

public:
	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	// Return the camera component
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	
private:
	UFUNCTION()
	void OnRep_IsFlashlightOn();

};
