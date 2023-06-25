// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ALSFunctionLibrary.generated.h"

struct FALSActorData;

/**
 * 
 */
UCLASS()
class ASCENTSAVESYSTEM_API UALSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 

	UFUNCTION(BlueprintCallable, Category = ALS)
	static bool TrySaveSceenshot(const FString& fileName, const int32 width = 640, const int32 height = 480);

	UFUNCTION(BlueprintCallable, Category = ALS)
	static UTexture2D* GetScreenshotByName(const FString& fileName);

	UFUNCTION(BlueprintCallable, Category = ALS)
	static bool ShouldSaveActor(AActor* actor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ALS)
	static bool IsNewGame(const UObject* WorldContextObject);

	UFUNCTION(Category = ALS)
	static bool IsSpecialActor(const UObject* WorldContextObject, const AActor* actor);

	UFUNCTION(Category = ALS)
	static void ExecuteFunctionsOnSavableComponents(const AActor* actorOwner, const FName& functionName);

	static void DeserializeActor(AActor* Actor, const FALSActorData& Record);

	static void DeserializeActorComponents(AActor* Actor, const FALSActorData& ActorRecord);

	static FALSActorData SerializeActor(AActor* actor);

	static void SerializeComponents(const AActor* Actor, FALSActorData& ActorRecord);

	static FString ConstructScreenshotPath(const FString& fileName);

};
