// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ALSSaveGame.h"
#include "ALSSaveGameSettings.generated.h"


/**
 * 
 */
UCLASS(config = Plugins, Defaultconfig, meta = (DisplayName = "Ascent Load & Save"))
class ASCENTSAVESYSTEM_API UALSSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
protected: 
	UALSSaveGameSettings(){}
		
	UPROPERTY(EditAnywhere, config, Category = "ALS | Defaults")
	TSubclassOf<class UALSSaveGame> SaveGameClass = UALSSaveGame::StaticClass();

	UPROPERTY(EditAnywhere, config, Category = "ALS | Defaults")
	FName OnComponentSavedFunctionName = "OnComponentSaved";

	UPROPERTY(EditAnywhere, config, Category = "ALS | Defaults")
	FName OnComponentLoadedFunctionName = "OnComponentLoaded";

	UPROPERTY(EditAnywhere, config, Category = "ALS | Defaults")
	FString SavesMetadata = "SaveMetadata";

	UPROPERTY(EditAnywhere, config, Category = "ALS | Screenshot")
	int32 SaveScreenWidth = 640;

	UPROPERTY(EditAnywhere, config, Category = "ALS | Screenshot")
	int32 SaveScreenHeight = 480;

public: 
	
	TSubclassOf<class UALSSaveGame> GetSaveGameClass() const {
		return SaveGameClass;
	}

	FString GetSaveMetadataName() const {
		return SavesMetadata;
	}

	int32 GetDefaultScreenshotHeight() const {
		return SaveScreenHeight;
	}

	int32 GetDefaultScreenshotWidth() const {
		return SaveScreenWidth;
	}

	FName GetOnComponentSavedFunctionName() const {
		return OnComponentSavedFunctionName;
	}

	FName GetOnComponentLoadedFunctionName() const {
		return OnComponentLoadedFunctionName;
	}


};

