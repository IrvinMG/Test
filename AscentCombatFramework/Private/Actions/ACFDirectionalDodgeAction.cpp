// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Actions/ACFDirectionalDodgeAction.h"
#include "Actors/ACFCharacter.h"
#include "Game/ACFFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

UACFDirectionalDodgeAction::UACFDirectionalDodgeAction()
{
    DodgeDirectionToMontageSectionMap.Add(EACFDirection::Front, FName("Front"));
    DodgeDirectionToMontageSectionMap.Add(EACFDirection::Back, FName("Back"));
    DodgeDirectionToMontageSectionMap.Add(EACFDirection::Right, FName("Right"));
    DodgeDirectionToMontageSectionMap.Add(EACFDirection::Left, FName("Left"));
    ActionConfig.bStopBehavioralThree = true;
    ActionConfig.MontageReproductionType = EMontageReproductionType::EMotionWarped;
}

FTransform UACFDirectionalDodgeAction::GetWarpTransform_Implementation()
{
    FRotator finalRot;
    FVector finalPos;


    if (CharacterOwner && animMontage) {
        if (FMath::IsNearlyZero(dodgeDirection.Size())) {
            FRotator rotateDirection;
            switch (defaultDodgeDirection) {
            case EACFDirection::Back:
                rotateDirection = FRotator(0.f, 180.f, 0.f);
                break;
            case EACFDirection::Front:
                rotateDirection = FRotator(0.f, 0.f, 0.f);
                break;
            case EACFDirection::Right:
                rotateDirection = FRotator(0.f, 270.f, 0.f);
                break;
            case EACFDirection::Left:
                rotateDirection = FRotator(0.f, 90.f, 0.f);
                break;
            }

            dodgeDirection = rotateDirection.RotateVector(CharacterOwner->GetActorForwardVector());
        }

        finalPos = UACFFunctionLibrary::GetPointAtDirectionAndDistanceFromActor(CharacterOwner, dodgeDirection, DodgeLength, ActionConfig.WarpInfo.bShowWarpDebug);

        if (ActionConfig.WarpInfo.RotationType == EMotionWarpRotationType::Facing) {
            finalRot = UKismetMathLibrary::FindLookAtRotation(CharacterOwner->GetActorLocation(), finalPos);
        } else {
            finalRot = CharacterOwner->GetActorForwardVector().Rotation();
        }
        finalRot.Pitch = 0.f;
        finalRot.Roll = 0.f;
    }

    return FTransform(finalRot, finalPos);
}

void UACFDirectionalDodgeAction::OnActionStarted_Implementation(const FString& contextString)
{
    

    bool bIsValid;
    UKismetStringLibrary::Conv_StringToVector(contextString, dodgeDirection, bIsValid);
    dodgeDirection.Normalize();

    if (FMath::IsNearlyZero(dodgeDirection.Size())) {
        finalDirection = defaultDodgeDirection;
    } else {
        finalDirection = UACFFunctionLibrary::GetDirectionFromInput(CharacterOwner, dodgeDirection);
    }

    if (!bIsValid) {
        UE_LOG(LogTemp, Warning, TEXT("INVALID Dodge Direaction - UACFDirectionalDodgeAction::OnActionStarted_Implementation "));

    }
    Super::OnActionStarted_Implementation();
}

void UACFDirectionalDodgeAction::OnActionEnded_Implementation()
{
    Super::OnActionEnded_Implementation();
    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        acfCharacter->SetIsImmortal(false);
    }
}

FName UACFDirectionalDodgeAction::GetMontageSectionName_Implementation()
{
    const FName* section = DodgeDirectionToMontageSectionMap.Find(finalDirection);

    if (section) {
        return *section;
    }

    return Super::GetMontageSectionName_Implementation();
}

void UACFDirectionalDodgeAction::OnSubActionStateEntered_Implementation()
{
    Super::OnSubActionStateEntered_Implementation();

    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        acfCharacter->SetIsImmortal(true);
    }
}

void UACFDirectionalDodgeAction::OnSubActionStateExited_Implementation()
{
    Super::OnSubActionStateExited_Implementation();

    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        acfCharacter->SetIsImmortal(false);
    }
}
