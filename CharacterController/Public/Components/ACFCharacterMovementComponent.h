// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ACFActionTypes.h"
#include "ACFCCTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"

#include "ACFCharacterMovementComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocomotionStateChanged, ELocomotionState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveStanceChanged, EMovementStance, stance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotationModeChanged, ERotationMode, rotMode);
/**
 *
 *	The base character movement component class used by this project.
 */
UCLASS()
class CHARACTERCONTROLLER_API UACFCharacterMovementComponent : public UCharacterMovementComponent {
    GENERATED_BODY()
public:
    UACFCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

    friend class UACFAnimInstance;

    // Returns the current ground info.  Calling this will update the ground info if it's out of date.
    UFUNCTION(BlueprintCallable, Category = ACF)
    const FCharacterGroundInfo& GetGroundInfo();

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetRotationRateYaw() const;

    UPROPERTY(BlueprintAssignable)
    FOnLocomotionStateChanged OnLocomotionStateChanged;

    UPROPERTY(BlueprintAssignable)
    FOnLocomotionStateChanged OnTargetLocomotionStateChanged;

    UPROPERTY(BlueprintAssignable)
    FOnMoveStanceChanged OnMovementStanceChanged;

    UPROPERTY(BlueprintAssignable)
    FOnRotationModeChanged OnRotationModeChanged;

    UFUNCTION(BlueprintPure, Category = ACF)
    float GetCharacterMaxSpeed() const { return CharacterMaxSpeed; }

    UFUNCTION(BlueprintPure, Category = ACF)
    EMovementStance GetCurrentMovementStance() const { return currentMovestance; }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsCharacterStrafing() const { return RotationMode == ERotationMode::EStrafing; }
    
    UFUNCTION(BlueprintPure, Category = ACF)
    ERotationMode GetRotationMode() const { return RotationMode ; }


    UFUNCTION(BlueprintPure, Category = ACF)
    bool GetCanMove() const { return bCanMove; }

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetCanMove(bool inCanMove);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE ELocomotionState GetCurrentLocomotionState() const { return currentLocomotionState; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetCharacterMaxSpeedByState(ELocomotionState State);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetRotationMode(ERotationMode inRotMode);



    UFUNCTION(BlueprintCallable, Category = ACF)
    void ResetStrafeMovement();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void ActivateLocomotionStance(EMovementStance locStance);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void DeactivateLocomotionStance(EMovementStance locStance);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void DeactivateCurrentLocomotionStance();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void AccelerateToNextState();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void BrakeToPreviousState();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void TurnAtRate(float Rate);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void LookUpAtRate(float Rate);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void TurnAtRateLocal(float Value);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void MoveForward(float Value);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void MoveRight(float Value);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetMoveForwardAxis() const { return MoveForwardAxis; }
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetMoveRightAxis() const { return MoveRightAxis; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector GetWorldMovementInputVector();

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector GetCameraMovementInputVector();
    //
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE ELocomotionState GetTargetLocomotionState() const { return targetLocomotionState.State; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector GetCurrentInputVector();

    UFUNCTION(BlueprintPure, Category = ACF)
    EACFDirection GetCurrentInputDirection();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FAimOffset GetAimOffset() const
    {
        return aimOffest;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ResetToDefaultLocomotionState();
    /*Sets the target locomotion State*/
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetLocomotionState(ELocomotionState State);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetLocomotionStateSpeed(ELocomotionState State, float speed);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetMontageReprodutionType(EMontageReproductionType repType);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EMontageReproductionType GetMontageReprodutionType() const { return reproductionType; }

    /*DEPRECATED SOON*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void StartSnapping(const FSnapConfiguration& snapConfiguration);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StopSnapping();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StartOverrideSpeedAndDirection(const FVector& direction);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StopOverrideSpeedAndDirection();

    UFUNCTION(BlueprintPure, Category = ACF)
    float CalculateCurrentSnapMultiplier();

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector CalculateCurrentSnapDirection(float deltaTime);
    //*END DEPRECATE


    virtual void SimulateMovement(float DeltaTime) override;

    virtual bool CanAttemptJump() const override;

    void SetReplicatedAcceleration(const FVector& InAcceleration);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Controller")
    float TurnRate = 75;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Controller")
    float LookUpRate = 75;


    /**Indicates if this character can Move */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "ACF | Movement")
    bool bCanMove = true;

    /**Indicates the default locomtionstate*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Movement")
    ELocomotionState DefaultState = ELocomotionState::EJog;

    /**Indicates if this character follows control rotation and strafes*/
    UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_IsStrafing, Category = "ACF | Movement")
    ERotationMode RotationMode;

    /**Indicates max speed for each locomtion state*/
    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadWrite, Category = "ACF | Movement")
    TArray<FACFLocomotionState> LocomotionStates;

    /**Movement stances like blocking, aiming, casting etc. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Movement")
    TArray<FMovStances> MovementStances;

    /*Indicates the max angular distance from the cone that allows your character to sprint
    (you cannot sprint backward)*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Movement")
    float SprintDirectionCone = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "ACF | Movement")
    float GroundTraceDistance = 3000.f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | DEPRECATED")
    float AnimationCurvesScalingFactor = 1.f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | DEPRECATED")
    FName SpeedCurveName = "Speed";

    UPROPERTY(Replicated)
    float CharacterMaxSpeed;

    float MoveForwardAxis;
    float MoveRightAxis;

    void UpdateCharacterMaxSpeed();

    UPROPERTY(Replicated)
    float targetAlpha = 0.f;

    UPROPERTY(ReplicatedUsing = OnRep_ReproductionType)
    EMontageReproductionType reproductionType = EMontageReproductionType::ERootMotion;

    UPROPERTY(ReplicatedUsing = OnRep_LocomotionStance)
    EMovementStance currentMovestance;

    UPROPERTY()
    class UACFAnimInstance* animInstance;

    UPROPERTY()
    class ACharacter* Character;

    UPROPERTY(ReplicatedUsing = OnRep_LocomotionState)
    FACFLocomotionState targetLocomotionState;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentLocomotionState)
    ELocomotionState currentLocomotionState;

    UFUNCTION()
    void OnRep_LocomotionState();

   UFUNCTION()
    void OnRep_CurrentLocomotionState();

    UFUNCTION()
    void HandleStateChanged(ELocomotionState newState);

    UFUNCTION()
    void OnRep_ReproductionType();

    UFUNCTION()
    void OnRep_LocomotionStance();

    UPROPERTY()
    bool bIsSprinting = false;

    void UpdateMaxSpeed();

    void UpdateLocomotion();

    //UPROPERTY(Replicated)
    FAimOffset aimOffest;

protected:
    virtual void InitializeComponent() override;

protected:
    FCharacterGroundInfo CachedGroundInfo;

    UPROPERTY(Transient)
    bool bHasReplicatedAcceleration = false;

private:
    UFUNCTION()
    void OnRep_IsStrafing();

    UFUNCTION(NetMulticast, Reliable)
    void ClientsSetRotationMode(const ERotationMode& inRotMode);

    void Internal_SetStrafe();

    ERotationMode defaultRotMode;

    /*DEPRECATED*/
    UPROPERTY()
    FSnapConfiguration currentSnapConfiguration;

    UPROPERTY()
    float snapMultiplier = 1.0;

    /*When your AnimReproductionType is set to SnapToTarget, this struct
    identifies the anim ideal distance and the maximum multiplier applied to the animation
    speed to magneticly snap on the target*/
    FSnapConfiguration defaultSnapConfiguration;

    void Snap(float deltaTime);

    FVector overrideDirection;

    void OverrideSpeedAndDirection();

    void OverrideSpeed();

    /* END DEPRECATED*/
};
