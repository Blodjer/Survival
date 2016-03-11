// Copyright 2016 GAMES ACADEMY GmbH

#pragma once

#include "Engine/GameInstance.h"
#include "OnlineBlueprintCallProxyBase.h"
#include "FindSessionsCallbackProxy.h"
#include "SurvivalGameInstance.generated.h"


UCLASS()
class SURVIVAL_API USurvivalGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	/* Delegates */
	// Delegate called when session created
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	// Delegate called when session started
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	// Delegate for searching for sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	// Delegate for entering a session
	FOnJoinSessionCompleteDelegate OnEnterSessionCompleteDelegate;
	// Delegate for ending a session
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;
	// Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/* Delegate Handles */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnEnterSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

public:
	USurvivalGameInstance();

	UFUNCTION(BlueprintPure, Category = SurvivalNetwork)
	bool IsOnlineSubystemReady();

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool CreateSession(bool bIsLAN = false, bool bIsPresence = true, FString MapName = "", int32 MaxNumPlayers = 6);

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool StartSession();

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool FindSessions(bool bIsLAN = false, bool bIsPresence = true, FString MapName = "");

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool EnterSession(const int32 SearchResultsIndex);

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool EndSession();

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	bool DestroySession();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void CreateSessionComplete(bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void StartSessionComplete(bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void FindSessionsComplete(bool bWasSuccessful, const TArray<FBlueprintSessionResult>& SearchResults);

	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void EnterSessionComplete(int32 Result);

	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void EndSessionComplete(bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = SurvivalNetwork)
	void DestroySessionComplete(bool bWasSuccessful);

protected:
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	// Function fired when a session create request has completed
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// Function fired when a session start request has completed
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	// Function fired when a session search query has completed
	void OnFindSessionsComplete(bool bWasSuccessful);

	// Delegate fired when a session enter request has completed
	void OnEnterSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// Delegate fired when ending a online session has completed
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	// Delegate fired when destroying an online session has completed
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = SurvivalNetwork)
	virtual void ClearAllDelegates();

private:
	AGameSession* GetGameSession() const;
	
};
