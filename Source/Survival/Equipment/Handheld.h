// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Handheld.generated.h"

UCLASS(Abstract, NotBlueprintable)
class SURVIVAL_API AHandheld : public AActor
{
	GENERATED_BODY()
	
public:
	AHandheld();

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	// Set the character who owns this weapon
	void SetOwnerCharacter(ASurvivalPlayerCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = Handheld)
	void Equip();

	UFUNCTION(BlueprintCallable, Category = Handheld)
	void UnEquip();

	// Is the weapon currently equipped
	UFUNCTION(BlueprintPure, Category = Handheld)
	bool IsEquipped() const;

protected:
	// Setup the input for this weapon. Use BindInputAction(...) to bind actions to the owner character
	virtual void SetupInputActions() {};
	
	// Bind an action to the owner character
	template<class UserClass>
	void BindInputAction(const FName ActionName, const EInputEvent KeyEvent, UserClass* Object, typename FInputActionHandlerSignature::TUObjectMethodDelegate<UserClass>::FMethodPtr Func)
	{
		if (OwnerInputComponent != nullptr)
		{
			InputActionBindings.Add(OwnerInputComponent->BindAction(ActionName, KeyEvent, Object, Func));
		}
	}

private:
	// First person mesh. Seen only by owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Handheld, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	// Thrid person mesh. Not visible for owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Handheld, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh3P;

	// The character actor that owns this handheld
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerCharacter)
	class ASurvivalPlayerCharacter* OwnerCharacter;

	// The input component from the owner
	UPROPERTY(Transient)
	class UInputComponent* OwnerInputComponent;

	// Actions binded from this handheld to the owner's input
	TArray<FInputActionBinding> InputActionBindings;

	// Is the weapon currently equipped
	bool bIsEquipped;

private:
	// Remove all input actions binded to from this handheld to the owner's input
	void ClearActionBindings();

private:
	UFUNCTION()
	void OnRep_OwnerCharacter();

public:
	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	// Return the character actor that owns this handheld
	FORCEINLINE ASurvivalPlayerCharacter* GetOwnerCharacter() const { return OwnerCharacter; }

};