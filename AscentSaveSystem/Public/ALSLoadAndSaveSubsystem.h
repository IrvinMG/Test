// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <Async/AsyncWork.h>
#include "Subsystems/GameInstanceSubsystem.h"
#include <Engine/World.h>
#include "ALSLoadTask.h"
#include "ALSSaveTask.h"
#include "ALSSaveInfo.h"
#include "Kismet/GameplayStatics.h"
#include "ALSSaveGameSettings.h"
#include "ALSFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "ALSLoadAndSaveSubsystem.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELoadingState : uint8
{
	EIdle ,
	ESaving, 
	ELoading
};

UCLASS()
class ASCENTSAVESYSTEM_API UALSLoadAndSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	/*Saves all the game world ina s ave game*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void SaveGameWorld(const FString& slotName, const FOnSaveFinished& saveCallback, const bool bSaveLocalPlayer = true, const bool bSaveScreenshot = true);

	/*Load the provided slot and open the saved map*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void LoadGameWorld(const FString& slotName, const FOnLoadFinished& loadCallback);


	/*Reloads current level from the provided slot*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void LoadCurrentLevel(const FString& slotName, const FOnLoadFinished& loadCallback, bool bReloadLocalPlayer = true);

	/*Loads the provided level from the provided slot*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void LoadLevelFromSaveGame(const FString& slotName, const FString& levelName, const FOnLoadFinished& loadCallback, bool bReloadLocalPlayer);

	/*Save Current Player Controller & Pawn in provided slot*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void SaveLocalPlayer(const FString& slotName);

	/*Reloads the player from the provided slot*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void LoadLocalPlayer(const FString& slotName);

	/*Save Player Controller & Pawn in provided slot. Usefull for multiplayer Games*/
	UFUNCTION(BlueprintCallable, Category = ALS)
	void SavePlayer(const FString& slotName, const FString& playerID, APlayerController* playerToSave);

	/*Reloads the player from the provided slot Usefull for multiplayer Games */
	UFUNCTION(BlueprintCallable, Category = ALS)
	void LoadPlayer(const FString& slotName, const FString& playerID, APlayerController* playerToLoad);
	
	UFUNCTION(BlueprintCallable, Category = ALS)
	void SetNewGame(bool inNewGame) {
		bIsNewGame = inNewGame;
	}

	UFUNCTION(BlueprintPure, Category = ALS)
	bool IsNewGame() const {
		return bIsNewGame;
	}

	UFUNCTION(BlueprintCallable, Category = ALS)
	bool TryGetSaveMetadata(const FString& slotName, FALSSaveMetadata& outSaveMetadata) const;

	UFUNCTION(BlueprintCallable, Category = ALS)
	class UALSSaveGame* LoadOrCreateSaveGame(const FString& slotName);

	UFUNCTION(BlueprintCallable, Category = ALS)
	class UALSSaveInfo* LoadOrCreateSaveInfo();

	UFUNCTION(BlueprintCallable, Category = ALS)
	TArray<FALSSaveMetadata> GetAllSaveGames() const;

	UFUNCTION(BlueprintCallable, Category = ALS)
	UTexture2D* GetScreenshotForSave(const FString& saveName) const;

	UFUNCTION(BlueprintPure, Category = ALS)
	ELoadingState GetSystemState() const {
		return systemState;
	}

	UFUNCTION(BlueprintPure, Category = ALS)
	FString GetCurrentSaveName() const {
		return saveNameInternal;
	}


	UFUNCTION(BlueprintPure, Category = ALS)
	class UALSSaveGame* GetCurrentSaveGame() const {
		return currentSavegame;
	}


	void FinishSaveWork(const bool bSuccess);

	void FinishLoadWork(const bool bSuccess);


private: 

	bool bIsNewGame = true;
	bool bIsLoading;

	UFUNCTION()
	void HandleLoadingFinished(UWorld* world);

	UFUNCTION()
	void HandleLoadCompleted(const FString& SaveSlot, const int32 UserIndex, USaveGame* LoadedSaveData);

	void AsyncLoadSaveGame(const FString& savegameName);

	void SerializeObject(UObject* objectToSerialize, FALSObjectData& outData);
	void DeserializeObject(UObject* settingsObject, const FALSObjectData& objectData);

	
	FOnLoadFinished onLoadedCallbackInternal;
	FOnSaveFinished onSaveFinishedInternal;
	FString saveNameInternal;

	UPROPERTY()
	ELoadingState systemState;

	FString currentSaveSlot;
	UPROPERTY()
	class UALSSaveGame* currentSavegame;
};

static void GFinishSave(UWorld* WorldContextObject,  bool bSuccess) {
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UALSLoadAndSaveSubsystem>()->
		FinishSaveWork(bSuccess);
}

static void GFinishLoad(UWorld* WorldContextObject, TArray<FALSActorData> ToBeSpawned,
	TMap<AActor*, FALSActorLoaded>  SavedActors, TArray<AActor*> ToBeDestroyed, bool bSuccess) {
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	if (bSuccess && saveSettings) {
		for (const auto& actor : ToBeDestroyed) {
			actor->Destroy();
		}
		FActorSpawnParameters SpawnInfo{};
		for (const auto& Record : ToBeSpawned) {
			//SpawnInfo.Name = Record.alsName;
			AActor* spawnedActor = WorldContextObject->SpawnActor(Record.Class, &Record.Transform, SpawnInfo);
			UALSFunctionLibrary::DeserializeActor(spawnedActor, Record);

			IALSSavableInterface::Execute_OnLoaded(spawnedActor);
			UALSFunctionLibrary::ExecuteFunctionsOnSavableComponents(spawnedActor,
				saveSettings->GetOnComponentLoadedFunctionName());
		}

		for (const auto& actorRec : SavedActors) {
			if (!actorRec.Key) {
				continue;
			}
			actorRec.Key->SetActorTransform(actorRec.Value.transform);
			IALSSavableInterface::Execute_OnLoaded(actorRec.Key);
			UALSFunctionLibrary::ExecuteFunctionsOnSavableComponents(actorRec.Key,
				saveSettings->GetOnComponentLoadedFunctionName());
		}
	}
	UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UALSLoadAndSaveSubsystem>()->
		FinishLoadWork(bSuccess);
}