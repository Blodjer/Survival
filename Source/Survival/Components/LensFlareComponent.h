// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Components/SceneComponent.h"
#include "LensFlareComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent))
class SURVIVAL_API ULensFlareComponent : public UParticleSystemComponent
{
	GENERATED_BODY()

public:
	ULensFlareComponent();

	virtual void InitializeComponent() override;

#if WITH_EDITORONLY_DATA
	virtual void OnRegister() override;
#endif

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = LensFlare, meta = (AllowPrivateAccess = "true"))
	UTexture* OverrideTexture;

	UPROPERTY(EditAnywhere, Category = LensFlare, meta = (AllowPrivateAccess = "true"))
	bool bIsSpotLight;

	UPROPERTY(EditAnywhere, Category = LensFlare, meta = (AllowPrivateAccess = "true"))
	bool bScreenSizeScaled;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* LensFlareMID;
};
