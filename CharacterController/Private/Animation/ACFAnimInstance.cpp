// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Animation/ACFAnimInstance.h"
#include "ACFCCFunctionLibrary.h"
#include "Animation/ACFAnimTypes.h"
#include "Animation/ACFIKLayer.h"
#include "Animation/ACFMovesetLayer.h"
#include "Animation/ACFOverlayLayer.h"
#include "Animation/ACFRiderLayer.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "KismetAnimationLibrary.h"
#include <Animation/AimOffsetBlendSpace1D.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>

UACFAnimInstance::UACFAnimInstance()
{
    IKLayer = UACFIKLayer::StaticClass();
}

void UACFAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SetReferences();

    bIsMoving = false;
}

void UACFAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    ResetToDefaultMoveset();
    ResetToDefaultOverlay();
    if (IKLayer && IKLayer != UACFIKLayer::StaticClass()) {
        LinkAnimClassLayers(IKLayer);
    }
}

void UACFAnimInstance::SetMoveset(const FGameplayTag& MovesetTag)
{
    FMoveset* movLayer = MovesetLayers.FindByKey(MovesetTag);
    if (movLayer && movLayer->Moveset) {
        if (currentMovesetInstance) {
            currentMovesetInstance->OnDeactivated();
        }
        LinkAnimClassLayers(movLayer->Moveset);
        currentMoveset = *movLayer;
        currentMovesetInstance = Cast<UACFMovesetLayer>(GetLinkedAnimLayerInstanceByClass(movLayer->Moveset));
        if (currentMovesetInstance) {
            currentMovesetInstance->OnActivated();
        }
    }
}

bool UACFAnimInstance::GetAnimationOverlay(const FGameplayTag& tag, FOverlayLayer& outOverlay)
{
    FOverlayLayer* outOv = OverlayLayers.FindByKey(tag);
    if (outOv) {
        outOverlay = *outOv;
        return true;
    }

    return false;
}

void UACFAnimInstance::SetRidingLayer(const FGameplayTag& mountTag)
{
    FRiderLayer* rider = RiderLayers.FindByKey(mountTag);
    if (rider && rider->RiderLayer) {
        if (currentRiderInstance) {
            currentRiderInstance->OnDeactivated();
        }
        LinkAnimClassLayers(rider->RiderLayer);
        currentRiderLayer = *rider;
        currentRiderInstance = Cast<UACFRiderLayer>(GetLinkedAnimLayerInstanceByClass(rider->RiderLayer));
        if (currentRiderInstance) {
            currentRiderInstance->OnActivated();
        }
    }
}

void UACFAnimInstance::SetAnimationOverlay(const FGameplayTag& overlayTag)
{
    FOverlayLayer* overlay = OverlayLayers.FindByKey(overlayTag);
    if (overlay && overlay->Overlay) {
        RemoveOverlay();
        LinkAnimClassLayers(overlay->Overlay);
        currentOverlay = *overlay;
        currentOverlayInstance = Cast<UACFOverlayLayer>(GetLinkedAnimLayerInstanceByClass(overlay->Overlay));
        if (currentOverlayInstance) {
            currentOverlayInstance->OnActivated();
        }
    } else {
        RemoveOverlay();
    }
}

void UACFAnimInstance::RemoveOverlay()
{
    UnlinkAnimClassLayers(currentOverlay.Overlay);
    currentOverlay.TagName = FGameplayTag();
    if (currentOverlayInstance) {
        currentOverlayInstance->OnDeactivated();
    }
}

bool UACFAnimInstance::GetMovesetByTag(const FGameplayTag& movesetTag, FMoveset& outMoveset) const
{
    const FMoveset* outOv = MovesetLayers.FindByKey(movesetTag);
    if (outOv) {
        outMoveset = *outOv;
        return true;
    }

    return false;
}

FVector UACFAnimInstance::CalculateRelativeAccelerationAmount() const
{
    if (FVector::DotProduct(MovementComp->GetCurrentAcceleration(), WorldSpeed) > 0.0f) {
        const float MaxAcc = CharacterOwner->GetCharacterMovement()->GetMaxAcceleration();
        ensure(MaxAcc != 0.f);
        return OwnerRotation.UnrotateVector(
            MovementComp->GetCurrentAcceleration().GetClampedToMaxSize(MaxAcc) / MaxAcc);
    }

    const float MaxBrakingDec = CharacterOwner->GetCharacterMovement()->GetMaxBrakingDeceleration();
    ensure(MaxBrakingDec != 0.f);
    return OwnerRotation.UnrotateVector(
        MovementComp->GetCurrentAcceleration().GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

void UACFAnimInstance::HandleTargetLocomotionStateChanged(ELocomotionState newState)
{
    bIsWalking = newState == ELocomotionState::EWalk;
}

void UACFAnimInstance::SetReferences()
{
    CharacterOwner = Cast<ACharacter>(TryGetPawnOwner());
    if (CharacterOwner) {
        MovementComp = Cast<UACFCharacterMovementComponent>(CharacterOwner->GetCharacterMovement());
        if (MovementComp) {
            bIsWalking = (MovementComp->GetTargetLocomotionState() == ELocomotionState::EWalk);
        } else {
            UE_LOG(LogTemp, Error, TEXT("Owner doesn't have ACFCharachterMovement Comp!!!!"));
        }
    }
}

void UACFAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    if (CharacterOwner && MovementComp) {
        if (bUpdateMovementData) {
            UpdateStateData(DeltaSeconds);
        }
        if (bUpdateLocationData) {
            UpdateLocation(DeltaSeconds);
        }
        if (bUpdateRotationData) {
            UpdateRotation(DeltaSeconds);
        }
        if (bUpdateSpeedData) {
            UpdateVelocity(DeltaSeconds);
        }
        if (bUpdateAccelerationData) {
            UpdateAcceleration(DeltaSeconds);
        }
        if (bUpdateAimData) {
            UpdateAimData(DeltaSeconds);
        }
        if (bUpdateJumpData) {
            UpdateJump(DeltaSeconds);
        }
        if (bUpdateLeaningData) {
            UpdateLeaning(DeltaSeconds);
        }
    }
}
void UACFAnimInstance::UpdateLocation(float deltatime)
{
    DisplacementDelta = UKismetMathLibrary::VSizeXY(GetOwningActor()->GetActorLocation() - OwnerLocation);
    OwnerLocation = GetOwningActor()->GetActorLocation();
    Displacement = UKismetMathLibrary::SafeDivide(DisplacementDelta, deltatime);
}

void UACFAnimInstance::UpdateRotation(float deltatime)
{
    PreviousRotation = OwnerRotation;
    OwnerRotation = CharacterOwner->GetActorRotation();
    YawDelta = OwnerRotation.Yaw - PreviousRotation.Yaw;
    YawSpeed = UKismetMathLibrary::SafeDivide(YawDelta, deltatime);
    const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(OwnerRotation, PreviousRotation);
    const float turn = delta.Yaw;
    TurnRate = FMath::FInterpTo(TurnRate, turn, deltatime, TurnRateSmoothing);
}

void UACFAnimInstance::UpdateLeaning(float deltatime)
{
    LeanAngle = UKismetMathLibrary::SafeDivide(YawSpeed, deltatime) * 0.01f * UKismetMathLibrary::Abs(NormalizedSpeed) * LeaningFactor;
}

void UACFAnimInstance::UpdateVelocity(float deltatime)
{
    LastSpeedDirectionWithOffset = SpeedDirectionWithOffset;
    LastSpeedDirection = SpeedDirection;
    PreviousSpeed = Speed;
    bWasMoving = !FMath::IsNearlyZero(Speed, IsMovingSpeedThreshold);
    WorldSpeed = CharacterOwner->GetVelocity();
    Speed = WorldSpeed.Size();
    WorldSpeed2D = WorldSpeed * FVector(1.f, 1.f, 0.f);
    LocalSpeed2D = OwnerRotation.UnrotateVector(WorldSpeed2D);
    Direction = UKismetAnimationLibrary::CalculateDirection(WorldSpeed2D, OwnerRotation);
    DirectionWithOffset = Direction - YawOffset;
    SpeedDirection = GetDirectionFromAngle(Direction, LastSpeedDirection, bWasMoving);
    SpeedDirectionWithOffset = GetDirectionFromAngle(DirectionWithOffset, LastSpeedDirectionWithOffset, bWasMoving);
    NormalizedSpeed = UKismetMathLibrary::SafeDivide(Speed, MovementComp->GetCharacterMaxSpeed());
    if (IsLocalPlayer()) {
        DeltaInputDirection = UKismetAnimationLibrary::CalculateDirection(MovementComp->GetLastInputVector(), CharacterOwner->GetActorRotation());
    } else {
        // We don't have inputs on server
        DeltaInputDirection = DirectionWithOffset;
    }
    bIsMoving = !FMath::IsNearlyZero(Speed, IsMovingSpeedThreshold);
}

bool UACFAnimInstance::IsLocalPlayer() const
{
    return CharacterOwner->IsLocallyControlled() && CharacterOwner->IsPlayerControlled();
}

ELocomotionState UACFAnimInstance::GetCurrentLocomotionState() const
{
    return MovementComp->GetCurrentLocomotionState();
}

ELocomotionState UACFAnimInstance::GetTargetLocomotionState() const
{
    return MovementComp->GetTargetLocomotionState();
}

EACFDirection UACFAnimInstance::GetDirectionFromAngle_Implementation(float angle, EACFDirection currentDirection, bool bUseCurrentDirection)
{
    const float absAngle = FMath::Abs(angle);
    float fwdZone = DirectionDeadZone;
    float bwdZone = DirectionDeadZone;

    if (bUseCurrentDirection) {
        switch (currentDirection) {
        case EACFDirection::Front:
            fwdZone *= 2.f;
            break;
        case EACFDirection::Back:
            bwdZone *= 2.f;
            break;
        default:
            break;
        }
    }
    if (absAngle <= (45.f + fwdZone)) {
        return EACFDirection::Front;
    }
    if (absAngle >= (135.f - bwdZone)) {
        return EACFDirection::Back;
    }
    if (angle > 0.f) {
        return EACFDirection::Right;
    }
    return EACFDirection::Left;
}

void UACFAnimInstance::UpdateAimData(float deltatime)
{
    //     AimOffset.Y = MovementComp->GetAimOffset().AimOffsetPitch;
    //     AimOffset.X = MovementComp->GetAimOffset().AimOffsetYaw - YawOffset;
    const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(
        CharacterOwner->GetControlRotation(), CharacterOwner->GetActorRotation());
    if (CharacterOwner->IsLocallyControlled()) {

        float yawtarget = delta.Yaw - YawOffset;
        float pitchtarget = delta.Pitch;
        if (yawtarget >= 135.f) {
            if (yawtarget < 90.f) {
                yawtarget = -yawtarget;
            } else {
                yawtarget = yawtarget - 180.0f;
            }

            pitchtarget = -pitchtarget;
        } else if (yawtarget <= -135.f) {
            if (yawtarget > -90.f) {
                yawtarget = -yawtarget;
            } else {
                yawtarget = yawtarget + 180.f;
            }
            pitchtarget = -pitchtarget;
        }

        AimOffset.X = FMath::FInterpTo(
            AimOffset.X, yawtarget, deltatime, AimOffsetInterpSpeed);

        AimOffset.Y = FMath::FInterpTo(AimOffset.Y, pitchtarget, deltatime,
            AimOffsetInterpSpeed);
    } else {
        AimOffset.Y = CharacterOwner->GetBaseAimRotation().Pitch;
        AimOffset.X = -YawOffset;
    }
}

void UACFAnimInstance::UpdateAcceleration(float deltatime)
{
    Acceleration = MovementComp->GetCurrentAcceleration().Size();
    Acceleration2D = MovementComp->GetCurrentAcceleration() * FVector(1.f, 1.f, 0.f);
    LocalAccel2D = OwnerRotation.UnrotateVector(Acceleration2D);
    bIsAccelerating = Acceleration > 0;
    NormalizedAccel = Acceleration2D;
    NormalizedAccel.Normalize();
    PivotDirection = UKismetMathLibrary::VLerp(PivotDirection, NormalizedAccel, .5f);
    PivotDirection.Normalize();
    const float accelerationAngle = UKismetAnimationLibrary::CalculateDirection(PivotDirection, OwnerRotation);
    AccelerationDirection = UACFCCFunctionLibrary::GetOppositeDirectionFromAngle(accelerationAngle);
}

void UACFAnimInstance::UpdateStateData(float deltatime)
{
    bIsOnGround = MovementComp->IsMovingOnGround();
    bWasCrouching = bIsCrouching;
    bIsCrouching = CharacterOwner->bIsCrouched;
    bCrouchChanged = bIsCrouching != bWasCrouching;
    bWalkStateChanged = bWasWalking != bIsWalking;
    bWasWalking = bIsWalking;
    bIsStrafing = MovementComp->IsCharacterStrafing();
    bIsWalking = MovementComp->GetTargetLocomotionState() == ELocomotionState::EWalk; // || MovementComp->GetCurrentLocomotionState() == ELocomotionState::EIdle;
}

void UACFAnimInstance::UpdateJump(float deltatime)
{
    // Check if the play is falling?
    bIsJumping = false;
    bIsFalling = false;
    bIsInAir = MovementComp->IsFalling();
    if (bIsInAir) {
        if (WorldSpeed.Z > 0.f) {
            bIsJumping = true;
        } else {
            bIsFalling = true;
        }
    }

    const FCharacterGroundInfo& GroundInfo = MovementComp->GetGroundInfo();
    GroundDistance = GroundInfo.GroundDistance;

    if (bIsJumping) {
        TimeToApex = UKismetMathLibrary::SafeDivide(-WorldSpeed.Z, MovementComp->GetGravityZ());
    } else {
        TimeToApex = 0.f;
    }
}

bool UACFAnimInstance::GetIsCrouching() const
{
    return bIsCrouching;
}

void UACFAnimInstance::ResetToDefaultMoveset()
{
    SetMoveset(UACFCCFunctionLibrary::GetMovesetTypeTagRoot());
}

void UACFAnimInstance::ResetToDefaultOverlay()
{
    SetAnimationOverlay(UACFCCFunctionLibrary::GetAnimationOverlayTagRoot());
}

void UACFAnimInstance::SetTurnInPlaceYawOffset(float inYawOffset)
{
    YawOffset = inYawOffset;
}
