// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "SurvivalGameInstance.h"
#include "OnlineBlueprintCallProxyBase.h"
#include "FindSessionsCallbackProxy.h"


USurvivalGameInstance::USurvivalGameInstance()
{
	// Bind functions
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnStartSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnFindSessionsComplete);
	OnEnterSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnEnterSessionComplete);
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnEndSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &USurvivalGameInstance::OnDestroySessionComplete);
}

bool USurvivalGameInstance::IsOnlineSubystemReady()
{
	class IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	return OnlineSub != nullptr ? true : false;
}

bool USurvivalGameInstance::CreateSession(bool bIsLAN, bool bIsPresence, FString MapName, int32 MaxNumPlayers)
{
	const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId();

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Fill in all the Session Settings that we want to use.
			SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			// Fill advanced Session Settings
			SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);

			// Set the CreateSession delegate handle
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			return Sessions->CreateSession(*UserId, GameSessionName, *SessionSettings);
		}
	}

	return false;
}

void USurvivalGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the CreateSession delegate handle
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		}
	}

	CreateSessionComplete(bWasSuccessful);
}

bool USurvivalGameInstance::StartSession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Set the StartSession delegate handle
			OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

			return Sessions->StartSession(GameSessionName);
		}
	}

	return false;
}

void USurvivalGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the StartSession delegate handle
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		// Notify players to start the online session
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			Iterator->Get()->ClientStartOnlineSession();
		}
	}

	StartSessionComplete(bWasSuccessful);
}

bool USurvivalGameInstance::FindSessions(bool bIsLAN, bool bIsPresence, FString MapName)
{
	const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId();

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Fill in all the SearchSettings
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->PingBucketSize = 100;

			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			SessionSearch->QuerySettings.Set(SETTING_MAPNAME, MapName, EOnlineComparisonOp::Equals);

			// Set the FindSessions delegate handle
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			return Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong call the Delegate Function directly
		OnFindSessionsComplete(false);
	}

	return false;
}

void USurvivalGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	TArray<FBlueprintSessionResult> SearchResultsBP;

	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the FindSessions delegate handle
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
			TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId();

			// Copy Search Result into blueprint struct
			for (FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
			{
				if (SearchResult.IsValid())
				{
					FString MapName = "%&$";
					SearchResult.Session.SessionSettings.Get(SETTING_MAPNAME, MapName);
					FString ExpectedMapName = "$&%";
					SessionSearch->QuerySettings.Get(SETTING_MAPNAME, ExpectedMapName);

					if (SearchResult.Session.OwningUserId != UserId
						&& MapName == ExpectedMapName
						&& SearchResult.Session.NumOpenPublicConnections >= 1)
					{
						FBlueprintSessionResult SearchResultBP = FBlueprintSessionResult();
						SearchResultBP.OnlineResult = SearchResult;
						SearchResultsBP.Add(SearchResultBP);
					}
				}
			}
		}
	}

	FindSessionsComplete(bWasSuccessful, SearchResultsBP);
}

bool USurvivalGameInstance::EnterSession(const int32 SearchResultsIndex)
{
	const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId();

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the JoinSession delegate handle
			OnEnterSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnEnterSessionCompleteDelegate);

			return Sessions->JoinSession(*UserId, GameSessionName, SessionSearch->SearchResults[SearchResultsIndex]);
		}
	}

	return false;
}

void USurvivalGameInstance::OnEnterSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the JoinSession delegate handle
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnEnterSessionCompleteDelegateHandle);

			APlayerController* const PlayerController = GetFirstLocalPlayerController();

			// Could let set the constructor a String for this but every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Green, FString::Printf(TEXT("Travel URL: %s"), *TravelURL));

				// Finally the ClienTravel
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	EnterSessionComplete(Result);
}

bool USurvivalGameInstance::EndSession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Notify players to start the online session
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				Iterator->Get()->ClientEndOnlineSession();
			}

			OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);

			return Sessions->EndSession(GameSessionName);
		}
	}

	return false;
}

void USurvivalGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the EndSession delegate handle
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
		}
	}

	EndSessionComplete(bWasSuccessful);
}

bool USurvivalGameInstance::DestroySession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			return Sessions->DestroySession(GameSessionName);
		}
	}

	return false;
}

void USurvivalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			ClearAllDelegates();
		}
	}

	DestroySessionComplete(bWasSuccessful);
}

void USurvivalGameInstance::ClearAllDelegates()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnEnterSessionCompleteDelegateHandle);
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}
}

AGameSession* USurvivalGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameMode* const GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return GameMode->GameSession;
		}
	}

	return nullptr;
}
