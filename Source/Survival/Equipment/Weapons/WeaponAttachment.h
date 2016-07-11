// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "GameFramework/Actor.h"
#include "WeaponAttachment.generated.h"

UCLASS(Abstract, NotBlueprintable)
class SURVIVAL_API AWeaponAttachment : public AActor
{
	GENERATED_BODY()
	
public:
	AWeaponAttachment();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh3P;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh1P() const { return Mesh1P; };

	FORCEINLINE UStaticMeshComponent* GetMesh3P() const { return Mesh3P; };
};
