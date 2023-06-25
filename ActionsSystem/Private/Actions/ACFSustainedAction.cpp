// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 


#include "Actions/ACFSustainedAction.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Actions/ACFBaseAction.h"

UACFSustainedAction::UACFSustainedAction()
{
	bBindActionToAnimation = false;
}

void UACFSustainedAction::ReleaseAction()
{
    const float time = UGameplayStatics::GetRealTimeSeconds(CharacterOwner);
    const float secondsHeld = time - startTime;
    OnActionReleased(secondsHeld);
	PlayActionSection(FinalSectionName);
}

void UACFSustainedAction::PlayActionSection(FName sectionName)
{
	if (ActionsManager) {
        BindAnimationEvents();
		MontageInfo.StartSectionName = sectionName;

		ActionsManager->PlayReplicatedMontage(MontageInfo);
		bIsExecutingAction = true;
	}
}

void UACFSustainedAction::OnActionReleased_Implementation(float elapsedTimeSeconds)
{
}

void UACFSustainedAction::Internal_OnActivated(class UACFActionsManagerComponent* actionmanger, class UAnimMontage* inAnimMontage, const FString& contextString)
{
    startTime = UGameplayStatics::GetRealTimeSeconds(actionmanger);
    Super::Internal_OnActivated(actionmanger, inAnimMontage, contextString);
}
