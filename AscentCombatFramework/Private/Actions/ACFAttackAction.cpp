// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Actions/ACFAttackAction.h"
#include "ATSBaseTargetComponent.h"
#include "ATSTargetPointComponent.h"
#include "Actions/ACFBaseAction.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Interfaces/ACFEntityInterface.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Controller.h>
#include <Kismet/KismetMathLibrary.h>
#include <TargetingSystem/Public/ATSBaseTargetComponent.h>
#include "Kismet/KismetSystemLibrary.h"

UACFAttackAction::UACFAttackAction()
{
    DamageToActivate = EDamageActivationType::EBoth;
    ActionConfig.MontageReproductionType = EMontageReproductionType::EMotionWarped;
}

bool UACFAttackAction::TryGetTransform(FTransform& outTranform) const
{
    const UATSBaseTargetComponent* targetComp = CharacterOwner->GetController()->FindComponentByClass<UATSBaseTargetComponent>();
    if (targetComp) {
        AActor* target = targetComp->GetCurrentTarget();

        if (target) {
            const float entityExtent = IACFEntityInterface::Execute_GetEntityExtentRadius(target);
           //const float ownerExtent = IACFEntityInterface::Execute_GetEntityExtentRadius(CharacterOwner);

            FVector ownerLoc = CharacterOwner->GetActorLocation();
            ownerLoc.Z = target->GetActorLocation().Z;
            const FVector diffVector = target->GetActorLocation() - ownerLoc;

            const float distance = CharacterOwner->GetDistanceTo(target);

            const float warpDistance = distance - entityExtent;// - ownerExtent;
            const FVector finalPos = UACFFunctionLibrary::GetPointAtDirectionAndDistanceFromActor(CharacterOwner, diffVector, warpDistance,false);

            FRotator finalRot = UKismetMathLibrary::FindLookAtRotation(CharacterOwner->GetActorLocation(), finalPos);
            finalRot.Roll = 0.f;
            finalRot.Pitch = 0.f;
            outTranform = FTransform(finalRot, finalPos);
            return true;
        }
 
    }
    return false;
}

void UACFAttackAction::OnTick_Implementation(float DeltaTime)
{
    if (bContinuousUpdate && ActionConfig.MontageReproductionType == EMontageReproductionType::EMotionWarped) {
        UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();
        if (motionComp) {
            FTransform targetPoint;
            if (TryGetTransform(targetPoint)) {         
                warpTrans.SetRotation(FMath::QInterpTo(warpTrans.GetRotation(), targetPoint.GetRotation(), DeltaTime, WarpMagnetismStrength));
                warpTrans.SetLocation(FMath::VInterpTo(warpTrans.GetLocation(), targetPoint.GetLocation(), DeltaTime, WarpMagnetismStrength));
                if (ActionConfig.WarpInfo.bShowWarpDebug) {
                    UKismetSystemLibrary::DrawDebugSphere(CharacterOwner, warpTrans.GetLocation(), 100.f, 12, FLinearColor::Yellow, 1.f, 1.f);
                }
  
                const FMotionWarpingTarget newTarget = FMotionWarpingTarget(ActionConfig.WarpInfo.SyncPoint, warpTrans);
                 motionComp->AddOrUpdateWarpTarget(newTarget);
            }
        }
    }
    Super::OnTick_Implementation(DeltaTime);
}

USceneComponent* UACFAttackAction::GetWarpTargetComponent_Implementation()
{
    return currentTargetComp;
}

FTransform UACFAttackAction::GetWarpTransform_Implementation()
{
    return warpTrans;
}

void UACFAttackAction::OnActionStarted_Implementation(const FString& contextString)
{
    bSuccesfulCombo = false;

    if (CharacterOwner && bCheckWarpConditions) {
        const UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();
        const UATSBaseTargetComponent* targetComp = CharacterOwner->GetController()->FindComponentByClass<UATSBaseTargetComponent>();
        if (motionComp && targetComp && animMontage) {
            AActor* target = targetComp->GetCurrentTarget();
            IACFEntityInterface* entity = Cast<IACFEntityInterface>(target);
            IACFEntityInterface* ownerEntity = Cast<IACFEntityInterface>(CharacterOwner);
            if (target && entity && CharacterOwner) {
                const float entityExtent = IACFEntityInterface::Execute_GetEntityExtentRadius(target);
               // const float ownerExtent = IACFEntityInterface::Execute_GetEntityExtentRadius(CharacterOwner);

                FVector ownerLoc = CharacterOwner->GetActorLocation();
                ownerLoc.Z = target->GetActorLocation().Z;
                const FVector diffVector = target->GetActorLocation() - ownerLoc;

                const float distance = CharacterOwner->GetDistanceTo(target);

                const float warpDistance = distance - entityExtent ;//- ownerExtent;
                const FVector finalPos = UACFFunctionLibrary::GetPointAtDirectionAndDistanceFromActor(CharacterOwner, diffVector, warpDistance, ActionConfig.WarpInfo.bShowWarpDebug);

                FRotator finalRot = UKismetMathLibrary::FindLookAtRotation(CharacterOwner->GetActorLocation(), target->GetActorLocation());
                finalRot.Pitch = 0.f;
                finalRot.Roll = 0.f;
                const FRotator deltaRot = finalRot - CharacterOwner->GetActorForwardVector().Rotation();
                if (maxWarpDistance > warpDistance
                    && warpDistance > minWarpDistance
                    && maxWarpAngle > FMath::Abs(deltaRot.Yaw)) {

                    warpTrans = FTransform(finalRot, finalPos);

                    currentTargetComp = targetComp->GetCurrentTargetPoint();
                    SetMontageReproductionType(EMontageReproductionType::EMotionWarped);
                } else {
                    SetMontageReproductionType(EMontageReproductionType::ERootMotion);
                }
            } else {
                SetMontageReproductionType(EMontageReproductionType::ERootMotion);
            }
        }
    }
    Super::OnActionStarted_Implementation();
}

void UACFAttackAction::OnActionEnded_Implementation()
{
    Super::OnActionEnded_Implementation();

    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter && ActionsManager) {
        acfCharacter->DeactivateDamage(DamageToActivate, TraceChannels);
    }
    if (!bSuccesfulCombo) {
        CurrentComboIndex = 0;
    }
}

FName UACFAttackAction::GetMontageSectionName_Implementation()
{

    if (animMontage) {
        const FName SectionName = animMontage->GetSectionName(CurrentComboIndex);

        if (SectionName != NAME_None) {
            return SectionName;
        } else {
            CurrentComboIndex = 0;
            return animMontage->GetSectionName(CurrentComboIndex);
        }
    }
    return NAME_None;
}

void UACFAttackAction::OnSubActionStateEntered_Implementation()
{
    Super::OnSubActionStateEntered_Implementation();

    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter && ActionsManager) {
        acfCharacter->ActivateDamage(DamageToActivate, TraceChannels);
    }
}

void UACFAttackAction::OnSubActionStateExited_Implementation()
{
    Super::OnSubActionStateExited_Implementation();

    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter && ActionsManager) {
        acfCharacter->DeactivateDamage(DamageToActivate, TraceChannels);
    }
}

void UACFAttackAction::OnActionTransition_Implementation(class UACFBaseAction* previousState)
{
    if (previousState && previousState->IsA(UACFAttackAction::StaticClass())) {
        if (animMontage && CurrentComboIndex >= animMontage->CompositeSections.Num()) {
            CurrentComboIndex = 0;
        } else {
            CurrentComboIndex++;
        }
        bSuccesfulCombo = true;
    } else {
        CurrentComboIndex = 0;
        bSuccesfulCombo = false;
    }
}
