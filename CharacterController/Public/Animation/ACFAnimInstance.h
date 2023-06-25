// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFActionTypes.h"
#include "ACFCCTypes.h"
#include "Animation/ACFAnimTypes.h"
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ACFAnimInstance.generated.h"

/**
 *
 */

UCLASS()
class CHARACTERCONTROLLER_API UACFAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UACFAnimInstance();
    /* begin play */
    virtual void NativeInitializeAnimation() override;

    virtual void NativeBeginPlay() override;

    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetDirection() { return Direction; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetNormalizedSpeed() const { return NormalizedSpeed; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetSpeed() const { return Speed; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetTurnRate() const { return TurnRate; }
  
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsLocalPlayer() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    ELocomotionState GetCurrentLocomotionState() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    ELocomotionState GetTargetLocomotionState() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool GetIsCrouching() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool GetIsInAir() const { return bIsInAir; }

    // MOVESETS
    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetAnimationOverlay(const FGameplayTag& tag, FOverlayLayer& outOverlay);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetAnimationOverlay(const FGameplayTag& overlayTag);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetRidingLayer(const FGameplayTag& overlayTag);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetMoveset(const FGameplayTag& MovesetTag);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void RemoveOverlay();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsVelocityOverrideByAnim() const { return bVelocityOverrideByAnim; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE UACFMovesetLayer* GetCurrentMoveset() const { return currentMovesetInstance; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetMovesetByTag(const FGameplayTag& movesetTag, FMoveset& outMoveset) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentMovesetTag() const { return currentMoveset.TagName; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentOverlay() const { return currentOverlay.TagName; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ResetToDefaultMoveset();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ResetToDefaultOverlay();
    // END MOVESETS

    // IK
    UFUNCTION(BlueprintPure, Category = ACF)
    bool GetEnableFootIK() const { return EnableFootIK; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetEnableFootIK(bool val) { EnableFootIK = val; }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool GetEnableHandIK() const { return EnableHandIK; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetEnableHandIK(bool val) { EnableHandIK = val; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    EACFDirection GetStartDirection() const { return DesiredStartDirection; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetStartDirection(EACFDirection val) { DesiredStartDirection = val; }
    // END IK

protected:
    // ----- CONFIG ---- //
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float IsMovingSpeedThreshold = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float YawOffsetLimit = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float DirectionDeadZone = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float LeaningFactor = .5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float TurnRateSmoothing = 2.f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Config")
    float AimOffsetInterpSpeed = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "TagName"), Category = "ACF | Movesets")
    TArray<FMoveset> MovesetLayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "TagName"), Category = "ACF | Movesets")
    TArray<FOverlayLayer> OverlayLayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "TagName"), Category = "ACF | Movesets")
    TArray<FRiderLayer> RiderLayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Movesets")
    TSubclassOf<class UACFIKLayer> IKLayer;

    // ----- READ ONLY VARIABLES ---- //
    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movesets")
    FMoveset currentMoveset;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movesets")
    FOverlayLayer currentOverlay;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movesets")
    FRiderLayer currentRiderLayer;

    // Instances
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    UACFMovesetLayer* currentMovesetInstance;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    UACFOverlayLayer* currentOverlayInstance;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    UACFRiderLayer* currentRiderInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateMovementData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateLocationData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateRotationData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateSpeedData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateAccelerationData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateAimData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateJumpData = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Updates")
    bool bUpdateLeaningData = true;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bCanUseAdditive = false;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bIsCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bWasCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bCrouchChanged;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bIsOnGround;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bIsWalking;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bWasWalking;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bWalkStateChanged;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Movement")
    bool bIsStrafing;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Location")
    float Displacement = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Location")
    float DisplacementDelta = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Location")
    FVector OwnerLocation;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Rotation")
    FRotator OwnerRotation;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Rotation")
    FRotator PreviousRotation;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Rotation")
    float YawSpeed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Rotation")
    float YawDelta = 0.f;

    UPROPERTY(BlueprintReadWrite, Category = "ACF | Rotation")
    float YawOffset = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    bool bWasMoving;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float Direction = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float DirectionWithOffset = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float Speed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float DeltaInputDirection = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float NormalizedSpeed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    float PreviousSpeed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    FVector WorldSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    FVector WorldSpeed2D;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    FVector LocalSpeed2D;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    EACFDirection SpeedDirection;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    EACFDirection SpeedDirectionWithOffset;

    
    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    EACFDirection LastSpeedDirection;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Velocity")
    EACFDirection LastSpeedDirectionWithOffset;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | AimOffset")
    FVector2D AimOffset;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | AimOffset")
    float TurnRate;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Jump")
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Jump")
    bool bIsJumping;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Jump")
    bool bIsFalling;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Jump")
    float GroundDistance = -1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Jump")
    float TimeToApex = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    bool bIsAccelerating;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    float Acceleration = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    FVector Acceleration2D;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    FVector NormalizedAccel;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    FVector LocalAccel2D;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    FVector PivotDirection;

    UPROPERTY(BlueprintReadWrite, Category = "ACF | Acceleration")
    float TimeSinceLastPivot;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    EACFDirection AccelerationDirection;

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Leaning")
    float LeanAngle;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ACF | IK")
    bool EnableFootIK;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ACF | IK")
    bool EnableHandIK;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class ACharacter* CharacterOwner;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class UACFCharacterMovementComponent* MovementComp;
    // ----- END READ ONLY VARIABLES ---- //

    // ----- DEPRECATED ---- //
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bVelocityOverrideByAnim;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    UAnimSequence* StopAnim;
    // ----- END DEPRECATED ---- //

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DeprecatedFunction, DeprecationMessage = "MOVED IN ACTIONS MANAGER COMP!"), Category = "ACF | DEPRECATED")
    TMap<FGameplayTag, FActionState> CommonActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DeprecatedFunction, DeprecationMessage = "MOVED IN ACTIONS MANAGER COMP!"), Category = "ACF | DEPRECATED")
    TMap<FGameplayTag, FActionsSet> MovesetsActions;

    //     UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), meta = (DeprecatedFunction, DeprecationMessage = "CHECK MOVESET LAYERS"), BlueprintReadWrite, Category = "ACF | DEPRECATED")
    //     TArray<FACFMoveset> Movesets;

    // ----- END CONFIG ---- //

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetTurnInPlaceYawOffset(float inYawOffset);

   UFUNCTION(BlueprintNativeEvent, Category = ACF)
    EACFDirection GetDirectionFromAngle(float angle, EACFDirection currentDirection, bool bUseCurrentDirection);

    UPROPERTY(BlueprintReadOnly, Category = "ACF | Acceleration")
    EACFDirection DesiredStartDirection;

private:
    void SetReferences();

    void UpdateLeaning(float deltatime);
    void UpdateLocation(float deltatime);
    void UpdateRotation(float deltatime);
    void UpdateVelocity(float deltatime);


    void UpdateAimData(float deltatime);
    void UpdateAcceleration(float deltatime);
    void UpdateStateData(float deltatime);
    void UpdateJump(float deltatime);

    FVector CalculateRelativeAccelerationAmount() const;

    UFUNCTION()
    void HandleTargetLocomotionStateChanged(ELocomotionState newState);
};
