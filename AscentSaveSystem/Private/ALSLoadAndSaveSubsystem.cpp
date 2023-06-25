// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ALSLoadAndSaveSubsystem.h"
#include "ALSSaveGameSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ALSSaveInfo.h"
#include "Engine/GameInstance.h"
#include "ALSFunctionLibrary.h"





void UALSLoadAndSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UALSLoadAndSaveSubsystem::HandleLoadingFinished);
	bIsNewGame = true;
}

void UALSLoadAndSaveSubsystem::Deinitialize()
{

}


void UALSLoadAndSaveSubsystem::SaveGameWorld(const FString& slotName, const FOnSaveFinished& saveCallback, const bool bSaveLocalPlayer  /*= true*/, const bool bSaveScreenshot /*= true*/)
{
	if (systemState != ELoadingState::EIdle) {
		saveCallback.ExecuteIfBound(false);
		UE_LOG(LogTemp, Warning, TEXT("You are already loading or saving!"));
		return;
	}
	onSaveFinishedInternal = saveCallback;
	systemState = ELoadingState::ESaving;
	(new FAutoDeleteAsyncTask<FSaveWorldTask>(slotName, GetWorld(), bSaveLocalPlayer, bSaveScreenshot))->StartBackgroundTask();
}


void UALSLoadAndSaveSubsystem::LoadGameWorld(const FString& slotName, const FOnLoadFinished& loadCallback)
{
	if (systemState != ELoadingState::EIdle) {
		loadCallback.ExecuteIfBound(false);
		UE_LOG(LogTemp, Warning, TEXT("You are already loading or saving!"));
		return;
	}

	FALSSaveMetadata saveToBeLoaded;
	if (TryGetSaveMetadata(slotName, saveToBeLoaded)) {
		saveNameInternal = slotName;
		onLoadedCallbackInternal = loadCallback;
		bIsNewGame = false;
		systemState = ELoadingState::ELoading;
		UGameplayStatics::OpenLevel(this, *saveToBeLoaded.MapToLoad);	
	}
	else {
		loadCallback.ExecuteIfBound(false);
	}
}


void UALSLoadAndSaveSubsystem::SaveLocalPlayer(const FString& slotName)
{
	UALSSaveGame* saveGame = LoadOrCreateSaveGame(slotName);
	if (!saveGame) {
		return;
	}
	APlayerController* playerCont = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const FALSActorData pcData = UALSFunctionLibrary::SerializeActor(playerCont);

	APawn* pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	const FALSActorData pawnData = UALSFunctionLibrary::SerializeActor(pawn);

	FALSPlayerData playerData(pcData, pawnData);
	saveGame->StoreLocalPlayer(playerData);
	UGameplayStatics::SaveGameToSlot(saveGame, slotName, 0);

}

void UALSLoadAndSaveSubsystem::LoadLocalPlayer(const FString& slotName)
{
	UALSSaveGame* saveGame = LoadOrCreateSaveGame(slotName);
	if (!saveGame) {
		return;
	}
	FALSPlayerData outData;
	saveGame->GetLocalPlayer(outData);
	APlayerController* playerCont = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	UALSFunctionLibrary::DeserializeActor(playerCont, outData.PlayerController);

	APawn* pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FALSActorData playerData;
	UALSFunctionLibrary::DeserializeActor(pawn, outData.Pawn);
}

void UALSLoadAndSaveSubsystem::SavePlayer(const FString& slotName, const FString& playerID, APlayerController* playerToSave)
{
	UALSSaveGame* saveGame = LoadOrCreateSaveGame(slotName);
	if (!saveGame) {
		return;
	}
	const FALSActorData pcData = UALSFunctionLibrary::SerializeActor(playerToSave);
	APawn* pawn = playerToSave->GetPawn();
	const FALSActorData pawnData = UALSFunctionLibrary::SerializeActor(pawn);

	FALSPlayerData playerData(pcData, pawnData);
	saveGame->StorePlayer(slotName, playerData);
	UGameplayStatics::SaveGameToSlot(saveGame, slotName, 0);
}

void UALSLoadAndSaveSubsystem::LoadPlayer(const FString& slotName, const FString& playerID, APlayerController* playerToLoad)
{
	UALSSaveGame* saveGame = LoadOrCreateSaveGame(slotName);
	if (!saveGame) {
		return;
	}
	if (!playerToLoad) {
		return;
	}
	FALSPlayerData outData;
	if (saveGame->TryGetPlayer(playerID, outData)) {
		UALSFunctionLibrary::DeserializeActor(playerToLoad, outData.PlayerController);
		UALSFunctionLibrary::DeserializeActor(playerToLoad->GetPawn(), outData.Pawn);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("There are no Players with the provided ID in SaveSlot !"));
	}
}

void UALSLoadAndSaveSubsystem::LoadCurrentLevel(const FString& slotName, const FOnLoadFinished& loadCallback, bool bReloadCrossLevelActors /*= false*/)
{
	const FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	LoadLevelFromSaveGame(slotName, levelName, loadCallback, bReloadCrossLevelActors);
}

void UALSLoadAndSaveSubsystem::LoadLevelFromSaveGame(const FString& slotName, const FString& levelName, const FOnLoadFinished& loadCallback, bool bReloadCrossLevelActors /*= false*/)
{
	if (systemState != ELoadingState::EIdle) {
		loadCallback.ExecuteIfBound(false);
		UE_LOG(LogTemp, Warning, TEXT("You are already loading or saving!"));
		return;
	}

	FALSSaveMetadata saveToBeLoaded;
	if (TryGetSaveMetadata(slotName, saveToBeLoaded)) {
		onLoadedCallbackInternal = loadCallback;
		bIsNewGame = false;
		systemState = ELoadingState::ELoading;
		saveNameInternal = slotName;
		AsyncLoadSaveGame(saveNameInternal);
	}
	else {
		loadCallback.ExecuteIfBound(false);
	}
}

bool UALSLoadAndSaveSubsystem::TryGetSaveMetadata(const FString& slotName, FALSSaveMetadata& outSaveMetadata) const
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	const UALSSaveInfo* saveMetadata = Cast<UALSSaveInfo>(UGameplayStatics::LoadGameFromSlot(saveSettings->GetSaveMetadataName(), 0));

	if (!saveMetadata) {
		return false;
	}

	return saveMetadata->TryGetSaveSlotData(slotName, outSaveMetadata);
}

TArray<FALSSaveMetadata> UALSLoadAndSaveSubsystem::GetAllSaveGames() const
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	const UALSSaveInfo* saveMetadata = Cast<UALSSaveInfo>(UGameplayStatics::LoadGameFromSlot(saveSettings->GetSaveMetadataName(), 0));

	if (saveMetadata) {
		return saveMetadata->GetSaveSlots();
	}

	return TArray<FALSSaveMetadata>();
}

UTexture2D* UALSLoadAndSaveSubsystem::GetScreenshotForSave(const FString& saveName) const
{
	return UALSFunctionLibrary::GetScreenshotByName(saveName);
}

void UALSLoadAndSaveSubsystem::FinishSaveWork(const bool bSuccess)
{
	onSaveFinishedInternal.ExecuteIfBound(bSuccess);
	systemState = ELoadingState::EIdle;
}

void UALSLoadAndSaveSubsystem::FinishLoadWork(const bool bSuccess)
{ 
	onLoadedCallbackInternal.ExecuteIfBound(bSuccess);

	systemState = ELoadingState::EIdle;
}

void UALSLoadAndSaveSubsystem::HandleLoadingFinished(UWorld* world)
{		
	if (systemState == ELoadingState::ELoading) {
		AsyncLoadSaveGame(saveNameInternal);
	}
}

void UALSLoadAndSaveSubsystem::HandleLoadCompleted(const FString& SaveSlot, const int32 UserIndex, USaveGame* LoadedSaveData)
{	
	if (!LoadedSaveData) {
		onLoadedCallbackInternal.ExecuteIfBound(false);
		return;
	}
	currentSaveSlot = SaveSlot;
	currentSavegame = Cast<UALSSaveGame>(LoadedSaveData);
	if (!currentSavegame) {
		onLoadedCallbackInternal.ExecuteIfBound(false);
		return;
	}
	(new FAsyncTask<FLoadWorldTask>(saveNameInternal, GetWorld(), UGameplayStatics::GetCurrentLevelName(GetWorld()), true))->StartBackgroundTask();
}

void UALSLoadAndSaveSubsystem::AsyncLoadSaveGame(const FString& savegameName)
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UALSLoadAndSaveSubsystem::HandleLoadCompleted);

	UGameplayStatics::AsyncLoadGameFromSlot(savegameName, 0, LoadedDelegate);
}

void UALSLoadAndSaveSubsystem::SerializeObject(UObject* objectToSerialize, FALSObjectData& outData)
{
	FALSObjectData Record = { objectToSerialize };

	FMemoryWriter MemoryWriter(Record.Data, true);
	FALSSaveGameArchive Archive(MemoryWriter, false);
	objectToSerialize->Serialize(Archive);
	outData = Record;
}

void UALSLoadAndSaveSubsystem::DeserializeObject(UObject* settingsObject, const FALSObjectData& objectData)
{	
	FMemoryReader MemoryReader(objectData.Data, true);
	FALSSaveGameArchive Archive(MemoryReader, false);
	settingsObject->Serialize(Archive);
}

UALSSaveGame* UALSLoadAndSaveSubsystem::LoadOrCreateSaveGame(const FString& slotName)
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	UALSSaveGame* saveGame = Cast<UALSSaveGame>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
	if (saveGame) {
		return saveGame;
	}

	return Cast<UALSSaveGame>(UGameplayStatics::CreateSaveGameObject(saveSettings->GetSaveGameClass()));
}


UALSSaveInfo* UALSLoadAndSaveSubsystem::LoadOrCreateSaveInfo()
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	UALSSaveInfo* saveMetadata = Cast<UALSSaveInfo>(UGameplayStatics::LoadGameFromSlot(saveSettings->GetSaveMetadataName(), 0));
	if (saveMetadata) {
		return saveMetadata;
	}

	return  Cast<UALSSaveInfo>(UGameplayStatics::CreateSaveGameObject(UALSSaveInfo::StaticClass()));
}
