// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Actions/ACFHitAction.h"
#include "Actions/ACFBaseAction.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFQuadrupedMovementComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "ActionsSystem/Public/ACFActionsFunctionLibrary.h"

UACFHitAction::UACFHitAction()
{
    HitDirectionToMontageSectionMap.Add(EACFDirection::Front, FName("Front"));
    HitDirectionToMontageSectionMap.Add(EACFDirection::Back, FName("Back"));
    HitDirectionToMontageSectionMap.Add(EACFDirection::Right, FName("Right"));
    HitDirectionToMontageSectionMap.Add(EACFDirection::Left, FName("Left"));
    ActionConfig.bStopBehavioralThree = true;
}

void UACFHitAction::OnActionStarted_Implementation(const FString& contextString)
{
    Super::OnActionStarted_Implementation(contextString);

	AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        damageReceived = acfCharacter->GetLastDamageInfo();
        UACFActionsManagerComponent* actionsMan = acfCharacter->GetActionsComponent();
        if (actionsMan) {
            actionsMan->StoreAction(UACFActionsFunctionLibrary::GetDefaultActionsState());
        }
    }
    if (ActionConfig.MontageReproductionType == EMontageReproductionType::ECurveOverrideSpeedAndDirection) {
        const FVector damageMomentum = UACFFunctionLibrary::GetActorsRelativeDirectionVector(damageReceived);
        if (acfCharacter) {
            UACFCharacterMovementComponent* locComp = acfCharacter->GetACFCharacterMovementComponent();
            if (locComp) {
				locComp->StartOverrideSpeedAndDirection(damageMomentum);
            }
        }
    }
}

void UACFHitAction::OnActionEnded_Implementation()
{
    Super::OnActionEnded_Implementation();
    if (ActionConfig.MontageReproductionType == EMontageReproductionType::ECurveOverrideSpeedAndDirection) {
		AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);
		UACFCharacterMovementComponent* locComp = acfCharacter->GetACFCharacterMovementComponent();
		if (locComp) {
			locComp->StopOverrideSpeedAndDirection();
		}
    }
}

FName UACFHitAction::GetMontageSectionName_Implementation()
{

    const EACFDirection dir = damageReceived.DamageDirection;

    const FName* section = HitDirectionToMontageSectionMap.Find(dir);

    if (section) {
        return *section;
    }

    return Super::GetMontageSectionName_Implementation();
}
