// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "CCMCameraFunctionLibrary.h"
#include "CCMPlayerCameraManager.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>




class ACCMPlayerCameraManager* UCCMCameraFunctionLibrary::GetLocalCinematicCameraManager(const UObject* WorldContextObject)
{
	ACCMPlayerCameraManager* _cameramanager = Cast <ACCMPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0));
	if (_cameramanager)
	{
		return _cameramanager;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Remember to add BP_AdvancedCameraManager to your PlayerController!"));
		return nullptr;
	}
}


void UCCMCameraFunctionLibrary::TriggerCameraEvent(const UObject* WorldContextObject, FName CameraEventName)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->TriggerCameraEvent(CameraEventName);
	}
}


void UCCMCameraFunctionLibrary::TriggerTimedCameraEvent(const UObject* WorldContextObject, FName CameraEventName, float duration)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->TriggerTimedCameraEvent(CameraEventName, duration);
	}
}

void UCCMCameraFunctionLibrary::StopCameraEvent(const UObject* WorldContextObject, FName CameraEventName)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->StopCameraEvent(CameraEventName);
	}
}



void UCCMCameraFunctionLibrary::LockCameraOnActor(const UObject* WorldContextObject,   AActor* ActorLookAt, ELockType locktype , float lockStrength )
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->LockCameraOnActor(ActorLookAt, locktype, lockStrength);
	}
}

void UCCMCameraFunctionLibrary::LockCameraOnComponent(const UObject* WorldContextObject, USceneComponent* ComponentLookAt, ELockType locktype, float lockStrength)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->LockCameraOnComponent(ComponentLookAt, locktype, lockStrength);
	}
}

void UCCMCameraFunctionLibrary::StopLockingCameraOnActor(const UObject* WorldContextObject)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->StopLookingActor();
	}
}

void UCCMCameraFunctionLibrary::ResetCameraPosition(const UObject* WorldContextObject, bool bInstantReset /*= false*/)
{
	ACCMPlayerCameraManager* _cameramanager = GetLocalCinematicCameraManager(WorldContextObject);
	if (_cameramanager)
	{
		_cameramanager->ResetCameraPosition(bInstantReset);
	}
}

// UDataTable* UCCMCameraFunctionLibrary::GetCameraEvents(const UObject* WorldContextObject)
// {
// 	UCCMDeveloperSettings* _settings = GetMutableDefault<UCCMDeveloperSettings>();
// 
// 	if (_settings)
// 	{
// 		return _settings->CameraMovements;
// 	}
// 	UE_LOG(LogTemp, Warning, TEXT("Remember to Setup CameraMovements Data table in Project Settings!"));
// 	return nullptr;
// }
// 
// UDataTable* UCCMCameraFunctionLibrary::GetCameraRotationEvents(const UObject* WorldContextObject)
// {
// 	UCCMDeveloperSettings* _settings = GetMutableDefault<UCCMDeveloperSettings>();
// 
// 	if (_settings)
// 	{
// 		return _settings->CameraRotationSettings;
// 	}
// 	UE_LOG(LogTemp, Warning, TEXT("Remember to Setup CameraMovements Data table in Project Settings!"));
// 	return nullptr;
// }

