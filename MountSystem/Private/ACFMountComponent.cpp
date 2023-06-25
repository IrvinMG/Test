// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACFMountComponent.h"
#include "ACFRiderComponent.h"
#include "Actors/ACFCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/ACFGroupAIComponent.h"
#include "GameFramework/Controller.h"

UACFMountComponent::UACFMountComponent()
{
}

// Sets default values for this component's properties
void UACFMountComponent::SetMounted(bool inMounted)
{
    bIsMounted = inMounted;
    if (GetPawnOwner() && rider) {
        if (bIsPlayerCompanion && bPossessMount) {
            const AController* riderContr = rider->GetController();
            const AController* mountContr = GetPawnOwner()->GetController();
           
            AACFCharacter* acfMountChar = Cast<AACFCharacter>(GetPawnOwner());
            if (riderContr && mountContr && acfMountChar) {
               
                if (inMounted) {
                    UACFGroupAIComponent* groupAi = mountContr->FindComponentByClass<UACFGroupAIComponent>();
                    if (groupAi) {
                        groupAi->RemoveAgentFromGroup(acfMountChar);
                    }
                } else {
                    UACFGroupAIComponent* groupAi = riderContr->FindComponentByClass<UACFGroupAIComponent>();
                    if (groupAi) {
                         groupAi->AddExistingCharacterToGroup(acfMountChar);
                    }
                }
            }

        } else {
            const AController* contr = GetPawnOwner()->GetController();
            if (contr) {
                UBehaviorTreeComponent* behavComp = GetPawnOwner()->GetController()->FindComponentByClass<UBehaviorTreeComponent>();
                if (behavComp) {
                    if (inMounted) {
                        behavComp->PauseLogic("Mounted");
                    } else {
                        behavComp->ResumeLogic("Mounted");
                    }
                }
            }
        }
    }
    Super::SetMounted(inMounted);
}
