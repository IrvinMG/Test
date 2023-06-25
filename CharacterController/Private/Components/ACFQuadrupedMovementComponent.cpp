// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFQuadrupedMovementComponent.h"

#include "ARSStatisticsComponent.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFActionsManagerComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include <Animation/AnimEnums.h>
#include <Components/ActorComponent.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>
#include "GameFramework/Character.h"

void UACFQuadrupedMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!movementInput.IsNearlyZero()) {
        AddInputVector(movementInput);
        movementInput = FVector(0.f);
    }
    if (!FMath::IsNearlyZero(rotationInput.Yaw) && !IsFalling()) {
        CharacterOwner->AddActorWorldRotation(rotationInput);
        rotationInput = FRotator(0.f);
    }


    if (bForceSpeedToForward && !IsFalling()) {
        const FVector forward = Character->GetActorForwardVector() * Velocity.Size();
        Velocity = FMath::VInterpTo(Velocity, forward, DeltaTime, SpeedToForwardInterpRate);
    }

    if (bAlignBodyWithGround && MovementMode == MOVE_Walking) {
        float pitch, roll;
        UKismetMathLibrary::GetSlopeDegreeAngles(GetOwner()->GetActorRightVector(), CurrentFloor.HitResult.ImpactNormal, GetOwner()->GetActorUpVector(), pitch, roll);

        if (UKismetMathLibrary::Abs(pitch) > UKismetMathLibrary::Abs(AlignmentLimit)) {
            pitch = AlignmentLimit;
        }  
        
        const float finalPitch = FMath::FInterpTo(CharacterOwner->GetActorRotation().Pitch, pitch, DeltaTime, AlignmentSpeed);
        const FRotator targetRot = FRotator(finalPitch, CharacterOwner->GetActorRotation().Yaw, CharacterOwner->GetActorRotation().Roll);
        
        CharacterOwner->SetActorRotation(targetRot);
    }
}


void UACFQuadrupedMovementComponent::Turn(float Value)
{
    if (CharacterOwner && Value != 0.f) {
        const float finalValue = Value * RotationRate.Yaw * UGameplayStatics::GetWorldDeltaSeconds(this);
        rotationInput = FRotator(0, finalValue, 0);
      //  const FVector Direction = CharacterOwner->GetActorForwardVector();
      //  const FVector finalRot = YawRotation.RotateVector(Direction);

        // AddInputVector(finalRot * 0.1f);
        // CharacterOwner->AddActorWorldRotation(YawRotation);
    }
}

void UACFQuadrupedMovementComponent::MoveForwardLocal(float Value)
{
    if (CharacterOwner && Value != 0.0f) {
        // get forward vector
        const FVector Direction = CharacterOwner->GetActorForwardVector();

        movementInput = Direction * Value;
        //  AddInputVector(movementInput);
    }
}
