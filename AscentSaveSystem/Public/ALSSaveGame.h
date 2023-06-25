// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ALSSaveTypes.h"
#include "ALSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTSAVESYSTEM_API UALSSaveGame : public USaveGame
{
	GENERATED_BODY()

private:

	UPROPERTY(SaveGame)
	TMap<FString, FALSLevelData> Levels;

	UPROPERTY(SaveGame)
	TMap<FString, FALSPlayerData> Players;

	UPROPERTY(SaveGame)
	FALSPlayerData LocalPlayer;

	UPROPERTY(SaveGame)
	TMap<FString, FALSActorData> ExtraActors;

public:

	void AddLevel(const FString& levelName, const FALSLevelData& levelData) {
		Levels.Add(levelName, levelData);
	}

	void StoreLocalPlayer(const FALSPlayerData& actorData) {
		LocalPlayer = actorData;
	}

	void GetLocalPlayer(FALSPlayerData& outData) {
		outData = LocalPlayer;
	}

	void StorePlayer(const FString& playerID, const FALSPlayerData& actorData) {
		Players.Add(playerID, actorData);
	}

	bool TryGetPlayer(const FString& playerID, FALSPlayerData& outData) {
		if (Players.Contains(playerID)) {
			outData = *(Players.Find(playerID));
			return true;
		}
		return false;
	}

	void StoreExtraActor(const FString& actorKey, const FALSActorData& actorData) {
		ExtraActors.Add(actorKey, actorData);
	}

	bool TryGetLevelData(const FString& levelName, FALSLevelData& outData) {
		if (Levels.Contains(levelName)) {
			outData = *(Levels.Find(levelName));
			return true;
		}
		return false;
	}

	bool TryGetExtraActorData(const FString& actorKey, FALSActorData& outData) {
		if (ExtraActors.Contains(actorKey)) {
			outData = *(ExtraActors.Find(actorKey));
			return true;
		}
		return false;
	}
	
	//Called before saving this slot
	UFUNCTION(BlueprintNativeEvent, Category = ALS)
	void OnSaved();

	//Called after loading this slot
	UFUNCTION(BlueprintNativeEvent, Category = ALS)
	void OnLoaded();
};
