// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 
/*
2018 PakyMan Prod. 
*/
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Engine/DataTable.h>
#include "CCMTypes.h"
#include "CCMCameraFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CINEMATICCAMERAMANAGER_API UCCMCameraFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static class ACCMPlayerCameraManager* GetLocalCinematicCameraManager(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static	void TriggerCameraEvent(const UObject* WorldContextObject, FName CameraEventName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static	void TriggerTimedCameraEvent(const UObject* WorldContextObject, FName CameraEventName, float duration);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static void StopCameraEvent(const UObject* WorldContextObject, FName CameraEventName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static void LockCameraOnActor(const UObject* WorldContextObject, AActor* ActorLookAt, ELockType locktype, float lockStrength = 5.f);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static void StopLockingCameraOnActor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static void LockCameraOnComponent(const UObject* WorldContextObject, USceneComponent* ComponentLookAt, ELockType locktype, float lockStrength);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = CCM)
	static void ResetCameraPosition(const UObject* WorldContextObject, bool bInstantReset = false);

};
