// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "LensFlareComponent.h"

ULensFlareComponent::ULensFlareComponent()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> LensFlareTemplate(TEXT("/Game/Survival/Effects/LensFlares/P_LensFlarePointLight.P_LensFlarePointLight"));
	SetTemplate(LensFlareTemplate.Object);
	
	bIsSpotLight = false;
	bScreenSizeScaled = false;
	DepthOffset = 0.0f;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITORONLY_DATA
	bTickInEditor = true;
#endif
}

void ULensFlareComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

#if WITH_EDITORONLY_DATA
void ULensFlareComponent::OnRegister()
{
	Super::OnRegister();

	UObject* LensFlareTemplate;
	if (bIsSpotLight)
	{
		LensFlareTemplate = StaticLoadObject(UParticleSystem::StaticClass(), this, TEXT("/Game/Survival/Effects/LensFlares/P_LensFlareSpotLight.P_LensFlareSpotLight"));
	}
	else
	{
		LensFlareTemplate = StaticLoadObject(UParticleSystem::StaticClass(), this, TEXT("/Game/Survival/Effects/LensFlares/P_LensFlarePointLight.P_LensFlarePointLight"));
	}

	SetTemplate(Cast<UParticleSystem>(LensFlareTemplate));
	LensFlareMID = UMaterialInstanceDynamic::Create(GetMaterial(0), this);
	SetMaterial(0, LensFlareMID);

	if (GetAttachParent())
	{
		SetVisibility(GetAttachParent()->IsVisible(), true);
		
		ULightComponent* LightParent = Cast<ULightComponent>(GetAttachParent());
		if (LensFlareMID && LightParent)
		{
			LensFlareMID->SetVectorParameterValue("Tint", LightParent->GetLightColor());
		}
	}

	if (OverrideTexture)
	{
		LensFlareMID->SetTextureParameterValue("FlareTexture", OverrideTexture);
	}
	LensFlareMID->SetScalarParameterValue("ScreenSizeScaled", bScreenSizeScaled ? 1.0f : 0.0f);
	LensFlareMID->SetScalarParameterValue("DepthOffset", DepthOffset);
}
#endif

#if WITH_EDITOR
void ULensFlareComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UObject* LensFlareTemplate;
	if (bIsSpotLight)
	{
		LensFlareTemplate = StaticLoadObject(UParticleSystem::StaticClass(), this, TEXT("/Game/Survival/Effects/LensFlares/P_LensFlareSpotLight.P_LensFlareSpotLight"));
	}
	else
	{
		LensFlareTemplate = StaticLoadObject(UParticleSystem::StaticClass(), this, TEXT("/Game/Survival/Effects/LensFlares/P_LensFlarePointLight.P_LensFlarePointLight"));
	}

	SetTemplate(Cast<UParticleSystem>(LensFlareTemplate));
	LensFlareMID = UMaterialInstanceDynamic::Create(GetMaterial(0), this);
	SetMaterial(0, LensFlareMID);

	if (OverrideTexture)
	{
		LensFlareMID->SetTextureParameterValue("FlareTexture", OverrideTexture);
	}
	LensFlareMID->SetScalarParameterValue("ScreenSizeScaled", bScreenSizeScaled ? 1.0f : 0.0f);
	LensFlareMID->SetScalarParameterValue("DepthOffset", DepthOffset);
}
#endif

void ULensFlareComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetAttachParent())
	{
		ULightComponent* LightParent = Cast<ULightComponent>(GetAttachParent());
		if (LensFlareMID && LightParent)
		{
			LensFlareMID->SetVectorParameterValue("Tint", LightParent->GetLightColor());

			bool bLightIsVisible = LightParent->IsVisible() && LightParent->Intensity > 0.0f;
			if (IsVisible() != bLightIsVisible)
			{
				SetVisibility(bLightIsVisible, true);
			}
		}
	}
}

