// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "BehavioralThree/ACFUpdatePatrolBTService.h"
#include "ACFAIController.h"
#include "Actors/ACFCharacter.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Pawn.h>

void UACFUpdatePatrolBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

    AACFAIController* aiController = Cast<AACFAIController>(OwnerComp.GetAIOwner());
    if (!aiController) {
        UE_LOG(LogTemp, Error, TEXT("GetController() is NULL - UACFUpdateCombatBTService"));
        return;
    }

    const APawn* CharOwner = aiController->GetPawn();
    if (!CharOwner) {
        UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UACFUpdateCombatBTService"));
        return;
    }
   
    const float distanceToTarget = FVector::Distance(CharOwner->GetActorLocation(), aiController->GetTargetPointLocationBK());
    aiController->SetTargetPointDistanceBK(distanceToTarget);
}
