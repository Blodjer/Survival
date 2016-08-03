// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Survival/Pickups/Pickup.h"
#include "Handheld.generated.h"

UENUM(BlueprintType)
enum class EHandheldType : uint8
{
	PrimaryWeapon,
	SecondaryWeapon,
	Gadget,
	Unknown
};

UCLASS(Abstract, NotBlueprintable)
class SURVIVAL_API AHandheld : public AActor
{
	GENERATED_BODY()
	
public:
	AHandheld();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	// Set the character who owns this weapon
	void SetOwnerCharacter(ASurvivalPlayerCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = Handheld)
	virtual void Equip();

	UFUNCTION(BlueprintCallable, Category = Handheld)
	virtual void UnEquip();
	
	void Drop(bool bIsWaste = false);

	virtual void OnCharacterStopUse() {};

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

	virtual void BeforeDrop() {};

	UFUNCTION(BlueprintPure, Category = Handheld)
	bool IsGameInputAllowed() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Handheld)
	EHandheldType Type;

private:
	// First person mesh. Seen only by owner.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Handheld, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	// Thrid person mesh. Not visible for owner.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Handheld, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh3P;

	// The character actor that owns this handheld
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_OwnerCharacter, meta = (AllowPrivateAccess = "true"))
	class ASurvivalPlayerCharacter* OwnerCharacter;

	// The input component from the owner
	UPROPERTY(Transient)
	class UInputComponent* OwnerInputComponent;

	// Actions binded from this handheld to the owner's input
	TArray<FInputActionBinding> InputActionBindings;

	// Is the weapon currently equipped
	bool bIsEquipped;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsWasted)
	bool bIsWaste;

	// Pickup to drop
	UPROPERTY(EditDefaultsOnly, Category = Handheld, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APickup> PickupClass;

private:
	// Remove all input actions binded to from this handheld to the owner's input
	void ClearActionBindings();

private:
	UFUNCTION()
	void OnRep_OwnerCharacter();

	UFUNCTION()
	void OnRep_IsWasted();

public:
	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	// Return the character actor that owns this handheld
	FORCEINLINE ASurvivalPlayerCharacter* GetOwnerCharacter() const { return OwnerCharacter; }

	// Is the weapon currently equipped
	UFUNCTION(BlueprintPure, Category = Handheld)
	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

	// Return the type of the handheld
	UFUNCTION(BlueprintPure, Category = Handheld)
	FORCEINLINE EHandheldType GetHandheldType() const { return Type; };

};
