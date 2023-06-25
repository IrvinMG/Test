// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include <Engine/DataTable.h>
#include <GameFramework/Actor.h>
#include "CCMTypes.h"
#include "CCMPlayerCameraManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSequenceStarted, const FName&, cameraSequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSequenceEnded, const FName&, cameraSequence);


UCLASS()
class CINEMATICCAMERAMANAGER_API ACCMPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public: 

	ACCMPlayerCameraManager();

protected: 
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateCamera(float deltaTime);
	UPROPERTY(EditAnywhere, Category = "CCM Camera")
	UDataTable* CameraMovements;

	UPROPERTY(EditAnywhere, Category = "CCM Camera")
	float CameraLockPitchUpperLimit = 75.f;
	 
	UPROPERTY(EditAnywhere, Category = "CCM Camera")
	float CameraLockPitchLowerLimit = 35.f;

	UPROPERTY(EditAnywhere, Category = "CCM Camera")
    FVector2D GlobalLockOffset = FVector2D();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCM Camera")
	TSubclassOf<ACameraActor> CameraActorClass;

	UPROPERTY(BlueprintReadOnly, Category = "CCM Camera")
	FCCMCameraMovementSettings FinalMov;

	UPROPERTY(BlueprintReadOnly, Category = "CCM Camera")
	class UCameraComponent* playerCamera;

	UPROPERTY(BlueprintReadOnly, Category = "CCM Camera")
	class USpringArmComponent* cameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "CCM Camera")
	bool bFollowPlayer = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bFollowPlayerWhenMoving"), Category = "CCM Camera")
	float FollowSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bFollowPlayerWhenMoving"), Category = "CCM Camera")
	float MaxFollowDeltaAngle = 180.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bFollowPlayerWhenMoving"), Category = "CCM Camera")
	bool bPrintDebugDeltaAngle = false;

	UPROPERTY()
	bool bIsPlayingSequence = false;

 	UPROPERTY()
    TObjectPtr<class ACameraActor> sequenceCameraActor;

	UPROPERTY()
	FCurrentSequence  currentSequence;
	
public:

	UPROPERTY(BlueprintAssignable, Category = "CCM")
	FOnCameraSequenceStarted OnCameraSequenceStarted;

	UPROPERTY(BlueprintAssignable, Category = "CCM")
	FOnCameraSequenceEnded OnCameraSequenceEnded;

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void TriggerCameraEvent(FName CameraEventName);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void TriggerTimedCameraEvent(FName CameraEventName, float duration);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void StopCameraEvent(FName CameraEventName);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void ResetCameraPosition(bool bInstantReset = false);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void LockCameraOnActor(class AActor* ActorLookAt, ELockType lockType, float lockStrength = 5.f );

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void LockCameraOnComponent(class USceneComponent* ComponentToLookAt, ELockType lockType, float lockStrength = 5.f);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void StopLookingActor();

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void SetFollowPlayer(bool bFollow, float inFollowSpeed = 1.f) {
		bFollowPlayer = bFollow;
		FollowSpeed = inFollowSpeed;
	}

	void UpdateCameraReferences();

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void TriggerCameraSequence(AActor* cameraOwner, FName CameraEventName);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void StopCurrentCameraSequence();

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void AddSequenceEventModifier(const FCCMSequenceEvent& modifier);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void RemoveSequenceEventModifier(const FCCMSequenceEvent& modifier);

	UFUNCTION(BlueprintCallable, Category = "CCM")
	void OverrideCameraReferences(class UCameraComponent* inPlayerCamera,	class USpringArmComponent* inCameraBoom);

private:

	void MoveCamera(float deltaTime);

	void SetCameraSequenceData();

	void UpdateLockOnTarget(float deltaTime);

	 void UpdateCameraSequence(float deltaTime);

	void LockCameraOnPosition(FVector localPos, float deltaTime);

	void StoreData();
	void RecoverStoredData();

	UPROPERTY()
	TMap<FName, FCCMCameraMovementSettings> _currentlyActiveCameraEvents;

	UPROPERTY()
	class APawn* playerCharacter;

	UPROPERTY()
	FVector _originalPos;

	UPROPERTY()
	float _originalFov;

	UPROPERTY()
	FRotator _originalRelativeRot;

	UPROPERTY()
	FVector _localPos;

	UPROPERTY()
	float _localFov;

	UPROPERTY()
	float _localTimeDilatation;

	UPROPERTY()
	ETargetLockType _localIsLockingAt;

	UPROPERTY()
	bool _localStrafing;

	UPROPERTY()
	float _lockStrength;

	UPROPERTY()
	bool _localOrientToMov;
	
	UPROPERTY()
	float _localArmLength;


	UPROPERTY()
	ETargetLockType targetLockType = ETargetLockType::ENone;

	UPROPERTY()
	ELockType currentLockType;
	
	UPROPERTY()
	class AActor* localtarget;

	UPROPERTY()
	class USceneComponent* localtargetComponent;

	UFUNCTION()
	void HandlePawnChanged(APawn* newPawn);

	void FollowPlayer(float deltaTime);

	UPROPERTY()
	FCCMCameraSequenceSettings currentSequenceSettings;


};
