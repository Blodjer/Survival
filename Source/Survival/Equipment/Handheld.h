// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "Handheld.generated.h"

UCLASS(Abstract, Blueprintable)
class SURVIVAL_API AHandheld : public AActor
{
	GENERATED_BODY()
	
public:
	AHandheld();

	void SetOwnerCharacter(ASurvivalPlayerCharacter* Character);

	void Equip();

	void UnEquip();

	bool IsEquipped() const;
	
private:
	// First person mesh. Seen only by owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	// Thrid person mesh. Not visible for owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh3P;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerCharacter)
	class ASurvivalPlayerCharacter* OwnerCharacter;

	bool bIsEquipped;

private:
	UFUNCTION()
	void OnRep_OwnerCharacter();

public:
	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	// Return the first person mesh
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

};
