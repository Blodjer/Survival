// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalUserSettings.h"

USurvivalUserSettings::USurvivalUserSettings()
{
	SetToDefaults();
}

USurvivalUserSettings* USurvivalUserSettings::LoadSettings()
{
	USurvivalUserSettings* SurvivalSaveGame = Cast<USurvivalUserSettings>(UGameplayStatics::LoadGameFromSlot("SurvivalUserSettings", 0));
	if (SurvivalSaveGame == nullptr)
	{
		SurvivalSaveGame = Cast<USurvivalUserSettings>(UGameplayStatics::CreateSaveGameObject(USurvivalUserSettings::StaticClass()));
	}

	return SurvivalSaveGame;
}

void USurvivalUserSettings::SaveSettings()
{
	UGameplayStatics::SaveGameToSlot(this, "SurvivalUserSettings", 0);
}

void USurvivalUserSettings::SetToDefaults()
{
	MouseSensitivityX = 0.5f;
	MouseSensitivityY = 0.5f;
	bInvertMouseY = false;
}

void USurvivalUserSettings::ApplyInputSettings()
{
	TArray<APlayerController*> PlayerControllers;
	GEngine->GetAllLocalPlayerControllers(PlayerControllers);

	for (APlayerController* PlayerController : PlayerControllers)
	{
		 if (PlayerController->PlayerInput)
		 {
			 FInputAxisProperties MouseAxisProps;

			 if (PlayerController->PlayerInput->GetAxisProperties(EKeys::MouseX, MouseAxisProps))
			 {
				 MouseAxisProps.Sensitivity = 0.01f + MouseSensitivityX * 0.13f;
				 PlayerController->PlayerInput->SetAxisProperties(EKeys::MouseX, MouseAxisProps);

			 }
			 if (PlayerController->PlayerInput->GetAxisProperties(EKeys::MouseY, MouseAxisProps))
			 {
				 MouseAxisProps.Sensitivity = 0.01f + MouseSensitivityY * 0.13f;
				 MouseAxisProps.bInvert = bInvertMouseY;
				 PlayerController->PlayerInput->SetAxisProperties(EKeys::MouseY, MouseAxisProps);
			 }
		 }
	}
}

void USurvivalUserSettings::SetMouseSensitivityX(float Value)
{
	MouseSensitivityX = Value;
}

void USurvivalUserSettings::SetMouseSensitivityY(float Value)
{
	MouseSensitivityY = Value;
}

void USurvivalUserSettings::SetInvertMouseY(bool bInvert)
{
	bInvertMouseY = bInvert;
}
