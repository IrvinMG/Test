// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ACFCharacterMovementComponent.h"
#include "ARSStatisticsComponent.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include <Camera/CameraComponent.h>
#include "Engine/World.h"

UACFCharacterMovementComponent::UACFCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bEditableWhenInherited = true;
    PrimaryComponentTick.bCanEverTick = true;

    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EIdle, 0.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EWalk, 250.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EJog, 500.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::ESprint, 650.f));

    SetIsReplicatedByDefault(true);
    currentLocomotionState = ELocomotionState::EIdle;
    currentMovestance = EMovementStance::EIdle;
}

float UACFCharacterMovementComponent::GetRotationRateYaw() const
{
    return RotationRate.Yaw;
}

void UACFCharacterMovementComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UACFCharacterMovementComponent, targetLocomotionState);
    DOREPLIFETIME(UACFCharacterMovementComponent, reproductionType);
    DOREPLIFETIME(UACFCharacterMovementComponent, targetAlpha);
    //DOREPLIFETIME(UACFCharacterMovementComponent, aimOffest);
    DOREPLIFETIME(UACFCharacterMovementComponent, LocomotionStates);
    DOREPLIFETIME(UACFCharacterMovementComponent, CharacterMaxSpeed);
    DOREPLIFETIME(UACFCharacterMovementComponent, bCanMove);
    DOREPLIFETIME(UACFCharacterMovementComponent, currentMovestance);
    DOREPLIFETIME(UACFCharacterMovementComponent, RotationMode);
    DOREPLIFETIME(UACFCharacterMovementComponent, currentLocomotionState);
}

void UACFCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
    if (bHasReplicatedAcceleration) {
        // Preserve our replicated acceleration
        const FVector OriginalAcceleration = Acceleration;
        Super::SimulateMovement(DeltaTime);
        Acceleration = OriginalAcceleration;
    } else {
        Super::SimulateMovement(DeltaTime);
    }
}

bool UACFCharacterMovementComponent::CanAttemptJump() const
{
    const UACFActionsManagerComponent* actionsManager = GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
    if (actionsManager) {
        return IsJumpAllowed() && (IsMovingOnGround() || IsFalling()) && !actionsManager->IsPerformingAction();
    }

    return IsJumpAllowed() && (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

void UACFCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
    bHasReplicatedAcceleration = true;
    Acceleration = InAcceleration;
}

void UACFCharacterMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    Character = Cast<ACharacter>(GetOwner());
    UpdateCharacterMaxSpeed();

    LocomotionStates.Sort();
}

void UACFCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    defaultRotMode = RotationMode;
    Internal_SetStrafe();
    SetLocomotionState(DefaultState);

    if (GetOwner()->HasAuthority()) {
        if (Character && Character->GetMesh()) {
            animInstance = Cast<UACFAnimInstance>(Character->GetMesh()->GetAnimInstance());
        } else {
            UE_LOG(LogTemp, Warning, TEXT("Owner not found!"));
        }
    }
}

void UACFCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateLocomotion();
//     switch (reproductionType) {
//     case EMontageReproductionType::ECurveOverrideSpeed:
//         OverrideSpeed();
//         break;
//     case EMontageReproductionType::ECurveSnapsOnTarget:
//         Snap(DeltaTime);
//         break;
//     case EMontageReproductionType::ECurveOverrideSpeedAndDirection:
//         OverrideSpeedAndDirection();
//         break;
//     default:
//         break;
//     }
}



void UACFCharacterMovementComponent::UpdateLocomotion()
{
    if (!Character) {
        return;
    }

    if (GetOwner() && !IsFalling() && animInstance && !animInstance->IsAnyMontagePlaying()) {
        if (GetGroundMovementMode() == MOVE_Walking) {
            for (int i = 0; i < LocomotionStates.Num() - 1; i++) {
                const float Speed = GetOwner()->GetVelocity().Size();
                if (FMath::IsNearlyZero(Speed) && currentLocomotionState != ELocomotionState::EIdle) {
                    HandleStateChanged(ELocomotionState::EIdle);
                }

                else if (LocomotionStates[i + 1].State != currentLocomotionState && Speed > LocomotionStates[i].MaxStateSpeed + 5.f && Speed <= LocomotionStates[i + 1].MaxStateSpeed + 5.f) {
                    HandleStateChanged(LocomotionStates[i + 1].State);
                }
            }

            if (currentLocomotionState == ELocomotionState::ESprint) {
                const float Direction = animInstance->GetDirection();
                if (FMath::Abs(Direction) > SprintDirectionCone) {
                    SetLocomotionState(ELocomotionState::EJog);
                }
            }
        } else {
            SetLocomotionState(ELocomotionState::EIdle);
        }
    }
}

const FCharacterGroundInfo& UACFCharacterMovementComponent::GetGroundInfo()
{
    if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame)) {
        return CachedGroundInfo;
    }

    if (MovementMode == MOVE_Walking) {
        CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
        CachedGroundInfo.GroundDistance = 0.0f;
    } else {
        const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
        check(CapsuleComp);

        const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
        const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
        const FVector TraceStart(CharacterOwner->GetActorLocation());
        const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - GroundTraceDistance - CapsuleHalfHeight));

        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ACFCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
        FCollisionResponseParams ResponseParam;
        InitCollisionParams(QueryParams, ResponseParam);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

        CachedGroundInfo.GroundHitResult = HitResult;
        CachedGroundInfo.GroundDistance = GroundTraceDistance;

        if (MovementMode == MOVE_NavWalking) {
            CachedGroundInfo.GroundDistance = 0.0f;
        } else if (HitResult.bBlockingHit) {
            CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
        }

        const FVector normal = HitResult.ImpactNormal;
        const FRotator actorRot = CharacterOwner->GetActorRotation();
        const FVector upVector = UKismetMathLibrary::GetUpVector(actorRot);
        const FVector rightVector = UKismetMathLibrary::GetUpVector(FRotator(0.f, actorRot.Yaw, 0.f));
        UKismetMathLibrary::GetSlopeDegreeAngles(rightVector, normal, upVector,
            CachedGroundInfo.SlopePitch, CachedGroundInfo.SlopeRoll);
    }

    CachedGroundInfo.LastUpdateFrame = GFrameCounter;

    return CachedGroundInfo;
}

void UACFCharacterMovementComponent::SetLocomotionState_Implementation(ELocomotionState State)
{
    FACFLocomotionState* locState = LocomotionStates.FindByKey(State);

    if (locState) {
        targetLocomotionState = *(locState);
        MaxWalkSpeed = GetCharacterMaxSpeedByState(State);
        targetLocomotionState.MaxStateSpeed = GetCharacterMaxSpeedByState(State);
        OnTargetLocomotionStateChanged.Broadcast(State);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Locomotion State inexistent"));
    }
}

void UACFCharacterMovementComponent::SetLocomotionStateSpeed_Implementation(
    ELocomotionState State, float speed)
{
    LocomotionStates.Remove(FACFLocomotionState(State, speed));
    LocomotionStates.AddUnique(FACFLocomotionState(State, speed));
    UpdateCharacterMaxSpeed();
}

void UACFCharacterMovementComponent::SetCanMove_Implementation(bool inCanMove)
{
    bCanMove = inCanMove;
}

float UACFCharacterMovementComponent::GetCharacterMaxSpeedByState(ELocomotionState State)
{
    FACFLocomotionState* state = LocomotionStates.FindByKey(State);
    if (state) {
        return state->MaxStateSpeed;
    }
    return 0.0f;
}

void UACFCharacterMovementComponent::UpdateCharacterMaxSpeed()
{
    if (Character->HasAuthority()) {
        float maxspeed = 0.0f;
        for (const FACFLocomotionState& state : LocomotionStates) {
            if (state.MaxStateSpeed >= maxspeed) {
                maxspeed = state.MaxStateSpeed;
            }
        }
        CharacterMaxSpeed = maxspeed;
    }
}

void UACFCharacterMovementComponent::OnRep_LocomotionState()
{
    MaxWalkSpeed = GetCharacterMaxSpeedByState(targetLocomotionState.State);
    targetLocomotionState.MaxStateSpeed = GetCharacterMaxSpeedByState(targetLocomotionState.State);
}

void UACFCharacterMovementComponent::OnRep_CurrentLocomotionState()
{
    OnLocomotionStateChanged.Broadcast(currentLocomotionState);
}

void UACFCharacterMovementComponent::HandleStateChanged(ELocomotionState newState)
{
    if (currentLocomotionState == newState) {
        return;
    }

    FACFLocomotionState* oldState = LocomotionStates.FindByKey(currentLocomotionState);
    FACFLocomotionState* nextState = LocomotionStates.FindByKey(newState);

    if (oldState && nextState && Character) {
        if (GetOwner()->HasAuthority()) {
            UARSStatisticsComponent* statComp = Character->FindComponentByClass<UARSStatisticsComponent>();

            statComp->RemoveAttributeSetModifier(oldState->StateModifier);
            statComp->AddAttributeSetModifier(nextState->StateModifier);
        }
    }
    currentLocomotionState = newState;
    OnLocomotionStateChanged.Broadcast(newState);
}

void UACFCharacterMovementComponent::OnRep_ReproductionType()
{
    ERootMotionMode::Type rootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;

    switch (reproductionType) {
    case EMontageReproductionType::ERootMotion:
    case EMontageReproductionType::ERootMotionScaled:

        rootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
        break;
    case EMontageReproductionType::ECurveOverrideSpeed:
    case EMontageReproductionType::ECurveSnapsOnTarget:
    case EMontageReproductionType::ECurveOverrideSpeedAndDirection:
        rootMotionMode = ERootMotionMode::IgnoreRootMotion;
        break;
    default:
        break;
    }

    if (Character && animInstance) {
        animInstance->SetRootMotionMode(rootMotionMode);
    }
}

void UACFCharacterMovementComponent::OnRep_LocomotionStance()
{
    OnMovementStanceChanged.Broadcast(currentMovestance);
}

void UACFCharacterMovementComponent::OnRep_IsStrafing()
{
    Internal_SetStrafe();
}

void UACFCharacterMovementComponent::ClientsSetRotationMode_Implementation(const ERotationMode& inRotMode)
{
    RotationMode = inRotMode;
    Internal_SetStrafe();
}

void UACFCharacterMovementComponent::Internal_SetStrafe()
{
    bOrientRotationToMovement = !(RotationMode == ERotationMode::EStrafing); // Character moves in the direction of input..
    bUseControllerDesiredRotation = RotationMode == ERotationMode::EStrafing;
    OnRotationModeChanged.Broadcast(RotationMode);
}

void UACFCharacterMovementComponent::SetRotationMode_Implementation(ERotationMode inRotMode)
{
    RotationMode = inRotMode;
    Internal_SetStrafe();
    ClientsSetRotationMode_Implementation(RotationMode);
}

void UACFCharacterMovementComponent::ResetStrafeMovement()
{
    SetRotationMode(defaultRotMode);
}

void UACFCharacterMovementComponent::ActivateLocomotionStance_Implementation(EMovementStance locStance)
{
    if (currentMovestance == locStance) {
        return;
    }

    if (MovementStances.Contains(locStance)) {
        if (currentMovestance != EMovementStance::EIdle) {
            DeactivateCurrentLocomotionStance();
        }
        FMovStances* stance = MovementStances.FindByKey(locStance);
        currentMovestance = locStance;
        SetLocomotionState(stance->locomotionState);
        OnMovementStanceChanged.Broadcast(currentMovestance);
    }
}

void UACFCharacterMovementComponent::DeactivateLocomotionStance_Implementation(EMovementStance locStance)
{
    if (currentMovestance == locStance) {
        DeactivateCurrentLocomotionStance();
    }
}

void UACFCharacterMovementComponent::DeactivateCurrentLocomotionStance_Implementation()
{
    currentMovestance = EMovementStance::EIdle;
    ResetToDefaultLocomotionState();
    OnMovementStanceChanged.Broadcast(currentMovestance);
}

void UACFCharacterMovementComponent::AccelerateToNextState_Implementation()
{
    LocomotionStates.Sort();

    const int32 actualindex = LocomotionStates.IndexOfByKey(currentLocomotionState);

    if (LocomotionStates.IsValidIndex(actualindex + 1)) {
        SetLocomotionState(LocomotionStates[actualindex + 1].State);
    }
}

void UACFCharacterMovementComponent::BrakeToPreviousState_Implementation()
{
    LocomotionStates.Sort();

    const int32 actualindex = LocomotionStates.IndexOfByKey(currentLocomotionState);

    if (LocomotionStates.IsValidIndex(actualindex - 1)) {
        SetLocomotionState(LocomotionStates[actualindex - 1].State);
    }
}

void UACFCharacterMovementComponent::TurnAtRate(float Rate)
{
    if (Character) {
        Character->AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
    }
}

void UACFCharacterMovementComponent::LookUpAtRate(float Rate)
{
    if (Character) {
        Character->AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
    }
}

void UACFCharacterMovementComponent::MoveForward(float Value)
{
    if (!bCanMove) {
        return;
    }
    // catch the forward axis

    MoveForwardAxis = Value;

    if (Character && Character->Controller && (MoveForwardAxis != 0.0f)) {
        // find out which way is forward
        const FRotator Rotation = Character->Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        AddInputVector(Direction * Value, false);
    }
}

void UACFCharacterMovementComponent::TurnAtRateLocal(float Value)
{
    if (Value != 0 && Character && Character->Controller) {
        const FRotator Rotation = Character->Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        const float RotationValue = Value * GetRotationRateYaw() * UGameplayStatics::GetWorldDeltaSeconds(this);
        const FRotator rotationDir(0, Rotation.Yaw + RotationValue, 0);
        Direction = rotationDir.RotateVector(Direction);

        AddInputVector(Direction * Value, false);
    }
}

void UACFCharacterMovementComponent::MoveRight(float Value)
{
    if (!bCanMove)
        return;

    // Catch the right axis

    MoveRightAxis = Value;

    if (Character && Character->Controller && (MoveRightAxis != 0.0f)) {
        // find out which way is right
        const FRotator Rotation = Character->Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        Character->AddMovementInput(Direction, Value);
    }
}

FVector UACFCharacterMovementComponent::GetWorldMovementInputVector()
{
    if (Character) {
        const FVector localDir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);

        FVector WorlDir = Character->GetActorForwardVector().Rotation().RotateVector(localDir);
        WorlDir.Normalize();
        return WorlDir;
    }
    return FVector();
}

FVector UACFCharacterMovementComponent::GetCameraMovementInputVector()
{
    if (Character) {
        const FVector localDir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);
        if (localDir.IsNearlyZero()) {
            return FVector::ZeroVector;
        }
        FVector WorlDir = Character->GetController()->GetControlRotation().RotateVector(localDir);
        WorlDir.Normalize();
        return WorlDir;
    }
    return FVector();
}

FVector UACFCharacterMovementComponent::GetCurrentInputVector()
{
    FVector dir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);
    dir.Normalize();
    return dir;
}

EACFDirection UACFCharacterMovementComponent::GetCurrentInputDirection()
{
    if (Character) {
        const FVector direction = GetCurrentInputVector();

        if (FMath::Abs(direction.X) > FMath::Abs(direction.Y)) {
            if (direction.X > 0) {
                return EACFDirection::Front;
            } else {
                return EACFDirection::Back;
            }
        } else {
            if (direction.Y > 0) {
                return EACFDirection::Right;
            } else {
                return EACFDirection::Left;
            }
        }
    }
    return EACFDirection::Front;
}

void UACFCharacterMovementComponent::ResetToDefaultLocomotionState()
{
    SetLocomotionState(DefaultState);
}

float UACFCharacterMovementComponent::CalculateCurrentSnapMultiplier()
{
    if (Character && currentSnapConfiguration.target) {
        float distance = FVector::Distance(Character->GetActorLocation(),
            currentSnapConfiguration.target->GetActorLocation());

        if (distance <= currentSnapConfiguration.maxSnapMultiplier * currentSnapConfiguration.preferredDistance) {
            snapMultiplier = distance / currentSnapConfiguration.preferredDistance;
        }

        return snapMultiplier;
    }
    return 1.f;
}

FVector UACFCharacterMovementComponent::CalculateCurrentSnapDirection(
    float deltaTime)
{
    if (currentSnapConfiguration.target) {
        const FVector targetDir = currentSnapConfiguration.target->GetActorLocation() - Character->GetActorLocation();
        return FMath::VInterpTo(Character->GetActorForwardVector(), targetDir,
            GetRotationRateYaw() * currentSnapConfiguration.RotationRateSnapMult,
            deltaTime)
            .GetSafeNormal();
    }
    return Character->GetActorForwardVector();
}

void UACFCharacterMovementComponent::StartOverrideSpeedAndDirection(
    const FVector& direction)
{
    if (direction == FVector::ZeroVector) {
        overrideDirection = CharacterOwner->GetActorForwardVector();
    }

    overrideDirection = direction.GetSafeNormal();
    SetMontageReprodutionType(EMontageReproductionType::ECurveOverrideSpeedAndDirection);
}

void UACFCharacterMovementComponent::StopOverrideSpeedAndDirection()
{
    SetMontageReprodutionType(EMontageReproductionType::ERootMotion);
}

void UACFCharacterMovementComponent::SetMontageReprodutionType_Implementation(
    EMontageReproductionType repType)
{
    reproductionType = repType;
    OnRep_ReproductionType();
}

void UACFCharacterMovementComponent::StartSnapping(
    const FSnapConfiguration& snapConfiguration)
{
    if (snapConfiguration.target && Character) {
        currentSnapConfiguration = snapConfiguration;
        SetMontageReprodutionType(EMontageReproductionType::ECurveSnapsOnTarget);
    }
}

void UACFCharacterMovementComponent::OverrideSpeedAndDirection()
{
    const FVector overrideVelocity = overrideDirection * animInstance->GetCurveValue(SpeedCurveName) * AnimationCurvesScalingFactor;
    Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Velocity.Z);
}

void UACFCharacterMovementComponent::OverrideSpeed()
{
    if (animInstance) {
        const float speed = animInstance->GetCurveValue(SpeedCurveName);
        Velocity = CharacterOwner->GetActorForwardVector() * animInstance->GetSpeed() * AnimationCurvesScalingFactor;
    }
}

void UACFCharacterMovementComponent::StopSnapping()
{
    currentSnapConfiguration = defaultSnapConfiguration;
    snapMultiplier = 1.0f;
}

void UACFCharacterMovementComponent::Snap(float deltaTime)
{
    if (animInstance) {
        const float speed = animInstance->GetCurveValue(SpeedCurveName);
        const FVector direction = CalculateCurrentSnapDirection(deltaTime);
        Velocity = direction * speed * CalculateCurrentSnapMultiplier() * AnimationCurvesScalingFactor;
    }
}
