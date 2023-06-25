// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "CCMPlayerCameraManager.h"
#include <GameFramework/Actor.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/Character.h>
#include <GameFramework/SpringArmComponent.h>
#include "CCMCameraFunctionLibrary.h"
#include <TimerManager.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "CCMCameraSplineComponent.h"
#include <Camera/CameraActor.h>
#include <CinematicCamera/Public/CineCameraComponent.h>
#include "Engine/Engine.h"


ACCMPlayerCameraManager::ACCMPlayerCameraManager()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	CameraActorClass = ACameraActor::StaticClass();
}



void ACCMPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	auto& eventNewPawn = controller->GetOnNewPawnNotifier();
	eventNewPawn.AddLambda([this](APawn* newPawn) {
		HandlePawnChanged(newPawn);
		}
	);

	UpdateCameraReferences();
	FinalMov.FovInterpSpeed = 0.f;
	FinalMov.InterpSpeed = 0.f;
}

void ACCMPlayerCameraManager::UpdateCamera(float deltaTime)
{
	Super::UpdateCamera(deltaTime);
	
	if (playerCamera && cameraBoom)
	{
		MoveCamera(deltaTime);			
		UpdateLockOnTarget(deltaTime);

		if (bIsPlayingSequence) {
			UpdateCameraSequence(deltaTime);
		}
	}

	FollowPlayer(deltaTime);
}

void ACCMPlayerCameraManager::UpdateLockOnTarget(float deltaTime)
{
	if (targetLockType != ETargetLockType::ENone &&  playerCharacter && playerCamera)
	{
		FVector localPos;
		if (targetLockType == ETargetLockType::EActor) {
			localPos = localtarget->GetActorLocation();
		}
		else {
			localPos = localtargetComponent->GetComponentLocation();
		}
		
		LockCameraOnPosition(localPos, deltaTime);
	}
}

void ACCMPlayerCameraManager::UpdateCameraSequence(float deltaTime)
{
	if (currentSequence.cameraSequenceComp && 
	currentSequence.currentTime < currentSequence.cameraSequenceComp->GetSplineLength() && 
	currentSequenceSettings.lookAtActor) {

		UCameraComponent* cameraComp = sequenceCameraActor->GetCameraComponent();
		if (!cameraComp) {
			return;
		}
		const float interpFov = FMath::FInterpTo(cameraComp->FieldOfView, currentSequenceSettings.CameraFov, deltaTime, currentSequenceSettings.FovInterpSpeed);
		cameraComp->SetFieldOfView(interpFov);
		float currentSpeed = currentSequenceSettings.CameraSpeed;	
		float const SplineLen = currentSequence.cameraSequenceComp->GetSplineLength();
		currentSequence.currentTime += currentSpeed * deltaTime * SplineLen;
		FVector const MountPos = currentSequence.cameraSequenceComp->GetLocationAtDistanceAlongSpline(currentSequence.currentTime , ESplineCoordinateSpace::World);
		sequenceCameraActor->SetActorLocation(MountPos);
		FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(MountPos, currentSequenceSettings.lookAtActor->GetActorLocation());
		FRotator smoothedLookAt = FMath::RInterpTo(sequenceCameraActor->GetActorRotation(), lookAtRot, deltaTime, currentSequenceSettings.CameraRotationsSpeed);
		sequenceCameraActor->SetActorRotation(smoothedLookAt);
 	}
	else {
		StopCurrentCameraSequence();
	}

}

void ACCMPlayerCameraManager::TriggerCameraSequence(AActor* cameraOwner, FName CameraEventName)
{
	if (cameraOwner && !bIsPlayingSequence) {

		TArray< UCCMCameraSplineComponent*> CameraSequences;
		cameraOwner->GetComponents<UCCMCameraSplineComponent>(CameraSequences, true);

		for (UCCMCameraSplineComponent* cameraSeq : CameraSequences) {
			if (cameraSeq->GetCameraSequenceName() == CameraEventName) {
				//StopLookingActor();
				currentSequence.Reset(cameraSeq);
				bIsPlayingSequence = true;
				break;
			}
		}

		if (currentSequence.cameraSequenceComp) {
 			SetCameraSequenceData();

			if (UKismetSystemLibrary::IsServer(this)) {
				UGameplayStatics::SetGlobalTimeDilation(this, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().TimeDilatation);
			}

			APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
			if (controller) {
				controller->SetViewTargetWithBlend(sequenceCameraActor, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().BlendSettingsTime);
			}		
		}
	}

}

void ACCMPlayerCameraManager::StopCurrentCameraSequence()
{
	bIsPlayingSequence = false;
	if (UKismetSystemLibrary::IsServer(this)) {
		UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
	}

	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	if (controller && sequenceCameraActor) {
		controller->SetViewTargetWithBlend(playerCharacter, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().OutBlendSettings);
	}
}

void ACCMPlayerCameraManager::AddSequenceEventModifier(const FCCMSequenceEvent& modifier)
{	
	if (!bIsPlayingSequence) {
		return;
	}
	if (modifier.bChangeTimeDilation && UKismetSystemLibrary::IsServer(this)) {
		UGameplayStatics::SetGlobalTimeDilation(this, modifier.TimeDilatation);
	}
	if (modifier.bEditCameraSpeed) {
		currentSequenceSettings.CameraSpeed = modifier.CameraSpeed;
	}
	if (modifier.bEditFov) {
		currentSequenceSettings.CameraFov = currentSequenceSettings.CameraFov += modifier.CameraFovOffset;
		currentSequenceSettings.FovInterpSpeed = modifier.FovOffsetInterpSpeed;
	}
	if (modifier.bSwitchLookAt) {
		currentSequenceSettings.lookAtActor = currentSequence.cameraSequenceComp->GetLookAtPointLocationByName(modifier.LookAtPoint);
	}

}

void ACCMPlayerCameraManager::RemoveSequenceEventModifier(const FCCMSequenceEvent& modifier)
{
	if (!bIsPlayingSequence) {
		return;
	}
	if (!currentSequence.cameraSequenceComp) {
		return;
	}

	if (modifier.bChangeTimeDilation) {
		UGameplayStatics::SetGlobalTimeDilation(this, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().TimeDilatation);
	}
	if (modifier.bEditCameraSpeed) {
		currentSequenceSettings.CameraSpeed = currentSequence.cameraSequenceComp->GetCameraSequenceSettings().CameraSpeed;
	}
	if (modifier.bEditFov) {
		currentSequenceSettings.CameraFov = currentSequence.cameraSequenceComp->GetCameraSequenceSettings().CameraFov;
	}
	if (modifier.bSwitchLookAt) {
		currentSequenceSettings.lookAtActor = currentSequence.cameraSequenceComp->GetLookAtPointLocationByName(currentSequence.cameraSequenceComp->GetCameraSequenceSettings().LookAtPoint);
	}

}

void ACCMPlayerCameraManager::OverrideCameraReferences(class UCameraComponent* inPlayerCamera, class USpringArmComponent* inCameraBoom)
{
	playerCamera = inPlayerCamera;
	cameraBoom = inCameraBoom;
}

void ACCMPlayerCameraManager::SetCameraSequenceData( )
{
	if (!sequenceCameraActor) {
		sequenceCameraActor = GetWorld()->SpawnActor<ACameraActor>(CameraActorClass );
	} 
	
	currentSequenceSettings = currentSequence.cameraSequenceComp->GetCameraSequenceSettings();
	currentSequenceSettings.lookAtActor = currentSequence.cameraSequenceComp->GetLookAtPointLocationByName(currentSequenceSettings.LookAtPoint);
	sequenceCameraActor->GetCameraComponent()->SetFieldOfView(currentSequenceSettings.CameraFov);
 	FVector startingLoc = currentSequence.cameraSequenceComp->GetLocationAtTime(0.f, ESplineCoordinateSpace::World);
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(startingLoc, currentSequenceSettings.lookAtActor->GetActorLocation());
 	sequenceCameraActor->SetActorLocationAndRotation(startingLoc, rot);

}

void ACCMPlayerCameraManager::LockCameraOnPosition(FVector localPos, float deltaTime)
{
	if (!playerCharacter || !playerCamera) {
		return;
	}
	

	if (!playerCharacter->GetController()) {
		return;
	}

	const FRotator OldRotation = playerCharacter->GetController()->GetControlRotation();
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(playerCamera->GetComponentLocation(), localPos);
	FRotator NewRotation = OldRotation;
	TargetRotation.Roll = OldRotation.Roll;
    TargetRotation.Pitch += GlobalLockOffset.Y;
    TargetRotation.Yaw += GlobalLockOffset.X;
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -CameraLockPitchLowerLimit, CameraLockPitchUpperLimit);

	if (currentLockType == ELockType::EYawOnly)	{
		TargetRotation.Pitch = OldRotation.Pitch;
	}
	NewRotation = FMath::RInterpTo(OldRotation, TargetRotation, deltaTime, _lockStrength);
	playerCharacter->GetController()->SetControlRotation(NewRotation);
}

void ACCMPlayerCameraManager::TriggerCameraEvent(FName CameraEventName)
{
	if (CameraEventName == NAME_None) {
		return;
	}

	if (CameraMovements)
	{
		FCCMCameraMovementSettings* _mov = CameraMovements->FindRow<FCCMCameraMovementSettings>(CameraEventName, "Searching for camera event");

		if (_currentlyActiveCameraEvents.Find(CameraEventName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera Event Already Triggered - Cinematic Camera Manager"));
			return;
		}

		if (_mov)
		{
			FinalMov += *(_mov);
			_currentlyActiveCameraEvents.Add(CameraEventName, *_mov);
		}
	}
}


void ACCMPlayerCameraManager::StopCameraEvent(FName CameraEventName)
{
	if (CameraEventName == NAME_None) {
		return;
	}

	if (_currentlyActiveCameraEvents.Contains(CameraEventName))
	{
		FCCMCameraMovementSettings* _mov = _currentlyActiveCameraEvents.Find(CameraEventName);
		if (_mov)
		{
			FinalMov -= *(_mov);
			_currentlyActiveCameraEvents.FindAndRemoveChecked(CameraEventName);
		}
	}
}


void ACCMPlayerCameraManager::TriggerTimedCameraEvent(FName CameraEventName, float duration)
{
	if (_currentlyActiveCameraEvents.Find(CameraEventName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera Event Already Triggered - Cinematic Camera Manager"));
		return;
	}

	UWorld* _world = GetWorld();
	if(_world)
	{
		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;
		TimerDel.BindUFunction(this, FName("StopCameraEvent"), CameraEventName);
		//Calling MyUsefulFunction after 5 seconds without looping
		_world->GetTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
		TriggerCameraEvent(CameraEventName);
	}

}



void ACCMPlayerCameraManager::ResetCameraPosition(bool bInstantReset /*= false*/)
{
	//StopCurrentCameraRotationEvent();

	TArray<FName> iterator;
	_currentlyActiveCameraEvents.GetKeys(iterator);

	TArray<FName> iteratorCopy = iterator;
	for (FName cameraevent : iteratorCopy)
	{
		StopCameraEvent(cameraevent);
	}

	if (bInstantReset && playerCamera && cameraBoom)
	{
		playerCamera->FieldOfView = _originalFov ;
		cameraBoom->SocketOffset = _originalPos;
	}
}


void ACCMPlayerCameraManager::LockCameraOnActor(AActor* ActorLookAt, ELockType lockType, float lockStrength )
{
	if (ActorLookAt)
	{
		localtarget = ActorLookAt;
		currentLockType = lockType;
		targetLockType = ETargetLockType::EActor;
		_lockStrength = lockStrength;
	}
}

void ACCMPlayerCameraManager::LockCameraOnComponent(class USceneComponent* ComponentToLookAt, ELockType lockType, float lockStrength /*= 5.f*/)
{
	if (ComponentToLookAt) 
	{
		localtargetComponent = ComponentToLookAt;
		currentLockType = lockType;
		targetLockType = ETargetLockType::EComponent;
		_lockStrength = lockStrength;
	}
}

void ACCMPlayerCameraManager::StopLookingActor()
{
	targetLockType = ETargetLockType::ENone;
}

void ACCMPlayerCameraManager::UpdateCameraReferences()
{
	if (PCOwner && CameraMovements )
	{
		ResetCameraPosition(true);
		playerCharacter = PCOwner->GetPawn();
		if (playerCharacter)
		{
			playerCamera = playerCharacter->FindComponentByClass<UCameraComponent>();
			cameraBoom = playerCharacter->FindComponentByClass<USpringArmComponent>();
			if (playerCamera && cameraBoom)
			{
				_originalFov = playerCamera->FieldOfView;
				_originalPos = cameraBoom->SocketOffset;
			
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Invalid Camera References !! - Cinematic Camera Manager"));

}

void ACCMPlayerCameraManager::MoveCamera(float deltaTime)
{
	if (cameraBoom)
	{
		FVector targetpos = FinalMov.CameraOffset + _originalPos;
		cameraBoom->SocketOffset = FMath::VInterpTo(cameraBoom->SocketOffset, targetpos, deltaTime, FinalMov.InterpSpeed);
	}
	

	if (playerCamera)
	{
		float finalfov = FinalMov.FOV + _originalFov;
		playerCamera->FieldOfView = FMath::FInterpTo(playerCamera->FieldOfView, finalfov, deltaTime, FinalMov.FovInterpSpeed);
	}
	
	if (FinalMov.bShakeLooping && PCOwner)
	{
		PCOwner->ClientStartCameraShake(FinalMov.Shake, FinalMov.ShakeIntensity);
	}
}



void ACCMPlayerCameraManager::StoreData()
{
	if (playerCharacter)
	{
		_localStrafing = playerCharacter->bUseControllerRotationYaw;
		_localIsLockingAt = targetLockType;
		_localArmLength = cameraBoom->TargetArmLength;
		_localTimeDilatation = 1.f;
		UCharacterMovementComponent* _charmov = playerCharacter->FindComponentByClass<UCharacterMovementComponent>();
		if(_charmov)
		{
			_localOrientToMov = _charmov->bOrientRotationToMovement;
		}	
	}
}

void ACCMPlayerCameraManager::RecoverStoredData()
{
	if (playerCharacter)
	{
		playerCharacter->bUseControllerRotationYaw = _localStrafing ;
		cameraBoom->TargetArmLength  = _localArmLength;
		UGameplayStatics::SetGlobalTimeDilation(this, _localTimeDilatation);

		UCharacterMovementComponent* _charmov = playerCharacter->FindComponentByClass<UCharacterMovementComponent>();
		if (_charmov)
		{
			_charmov->bOrientRotationToMovement  = _localOrientToMov;
		}
	}
}

void ACCMPlayerCameraManager::HandlePawnChanged(APawn* newPawn)
{
	UpdateCameraReferences();
}

void ACCMPlayerCameraManager::FollowPlayer(float deltaTime)
{
    if (bFollowPlayer && playerCharacter && playerCharacter->GetController()  && targetLockType == ETargetLockType::ENone) {
			const FRotator OldRotation = playerCharacter->GetController()->GetControlRotation();
			const FRotator TargetRotation = playerCharacter->GetActorForwardVector().Rotation();
			const FRotator NewRotation = FMath::RInterpTo(OldRotation, TargetRotation, deltaTime, FollowSpeed);

			 const float deltaAngle = NewRotation.Yaw - OldRotation.Yaw;
			 if (bPrintDebugDeltaAngle) {
				 GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Yellow, FString::Printf(TEXT("Rot Dif: %f"),
					 deltaAngle));
			 }
			 if (deltaAngle < MaxFollowDeltaAngle) {
				 playerCharacter->GetController()->SetControlRotation(FRotator(OldRotation.Pitch,NewRotation.Yaw, OldRotation.Roll));
			}
	}
}
