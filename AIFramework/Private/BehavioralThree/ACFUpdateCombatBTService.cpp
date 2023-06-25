// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "BehavioralThree/ACFUpdateCombatBTService.h"
#include "ACFAIController.h"
#include "Components/ACFCombatBehaviourComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include <AI/NavigationSystemBase.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Actor.h>
#include <Navigation/PathFollowingComponent.h>
#include <NavigationSystem.h>

void UACFUpdateCombatBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();

    aiController = Cast<AACFAIController>(OwnerComp.GetAIOwner());

    if (aiController == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("UACFUpdateCombatBTService get controller failed"));
        return;
    }

    targetActor = aiController->GetTargetActorBK();
    if (targetActor == nullptr) {
        if (!aiController->RequestAnotherTarget()) {
            aiController->ResetToDefaultState();
        }
        return;
    }

    CharOwner = Cast<AACFCharacter>(aiController->GetPawn());

    if (CharOwner == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UACFUpdateCombatBTService"));
        return;
    }

    if (targetActor->GetDistanceTo(CharOwner) > aiController->GetLoseTargetDistance()) {
        aiController->SetTarget(nullptr);
        aiController->ResetToDefaultState();
        return;
    }
    UpdateCombat();
}

void UACFUpdateCombatBTService::UpdateCombat()
{
    float distanceToTarget;
    ACharacter* targetChar = Cast<ACharacter>(targetActor);
    if (targetChar) {
        distanceToTarget = UACFFunctionLibrary::CalculateDistanceBetweenCharactersExtents(CharOwner, targetChar);
    } else {
        distanceToTarget = CharOwner->GetDistanceTo(targetActor);
    }

    aiController->SetTargetActorDistanceBK(distanceToTarget);

    if (distanceToTarget > aiController->GetLoseTargetDistance()) {
        aiController->SetTarget(nullptr);
    }

    combatBehav = aiController->GetCombatBehavior();

    if (combatBehav) {
        EAICombatState combatState = combatBehav->GetBestCombatStateByTargetDistance(distanceToTarget); // aiController->GetCombatStateBK();

        if (combatState != aiController->GetCombatStateBK()) {
            aiController->SetCombatStateBK(combatState);
        }
    }
}
