// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACFAIController.h"
#include "Groups/ACFAIGroupSpawner.h"
#include "Components/ACFAIPatrolComponent.h"
#include "Components/ACFCombatBehaviourComponent.h"
#include "Components/ACFCommandsManagerComponent.h"
#include "Components/ACFGroupAIComponent.h"
#include "Components/ACFThreatManagerComponent.h"
#include "ATSAITargetComponent.h"
#include "ATSTargetingComponent.h"
#include "Actors/ACFActor.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Components/ACFInteractionComponent.h"
#include "Components/ACFQuadrupedMovementComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFGameState.h"
#include "Game/ACFTypes.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Groups/ACFGroupAgentInterface.h"
#include <AI/NavigationSystemBase.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Bool.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Enum.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Int.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include <BehaviorTree/Blackboard/BlackboardKey.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BlackboardData.h>
#include <Components/SplineComponent.h>
#include <Engine/Engine.h>
#include <Navigation/CrowdFollowingComponent.h>
#include <Navigation/PathFollowingComponent.h>
#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AIPerceptionTypes.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AISense_Hearing.h>

AACFAIController::AACFAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
    CommandsManagerComp = CreateDefaultSubobject<UACFCommandsManagerComponent>(TEXT("CommandManager"));
    TargetingComponent = CreateDefaultSubobject<UATSAITargetComponent>(TEXT("TargetComp"));
    //	CombatBehaviorComponent = CreateDefaultSubobject<UACFCombatBehaviourComponent>(TEXT("CombatBehav"));
    ThreatComponent = CreateDefaultSubobject<UACFThreatManagerComponent>(TEXT("ThreatManager"));
}

void AACFAIController::OnPossess(APawn* possPawn)
{
    Super::OnPossess(possPawn);
    CharacterOwned = Cast<AACFCharacter>(possPawn);
    if (!CharacterOwned) {
        UE_LOG(LogTemp, Error, TEXT("AACFAIController possess non - AACEnemyCharacter type, %s"), *possPawn->GetName());
        return;
    }

    CharacterOwned->OnDamageReceived.AddDynamic(this, &AACFAIController::HandleCharacterHit);
    CharacterOwned->GetDamageHandlerComponent()->OnOwnerDeath.AddDynamic(this, &AACFAIController::HandleCharacterDeath);

    if (!BehaviorTree) {
        UE_LOG(LogTemp, Warning, TEXT("This AACEnemyCharacter should be assigned with a behavior Tree, %s"), *possPawn->GetName());
        return;
    }

    UBlackboardData* const bbData = BehaviorTree->BlackboardAsset;
    if (bbData == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("This behavior Tree should be assigned with a blackborad, %s"), *possPawn->GetName());
        return;
    }

  
    BlackboardComponent->InitializeBlackboard(*bbData);
    Blackboard = BlackboardComponent;

    
    // Fetch the Keys
    targetActorKey = BlackboardComponent->GetKeyID("TargetActor");
    targetPointLocationKey = BlackboardComponent->GetKeyID("TargetLocation");
    targetActorDistanceKey = BlackboardComponent->GetKeyID("TargetActorDistance");
    aiStateKey = BlackboardComponent->GetKeyID("AIState");
    targetDistanceKey = BlackboardComponent->GetKeyID("TargetLocationDistance");
    commandDurationTimeKey = BlackboardComponent->GetKeyID("CommandDuration");
    groupLeaderKey = BlackboardComponent->GetKeyID("GroupLeader");
    combatStateKey = BlackboardComponent->GetKeyID("CombatState");
    isPausedKey = BlackboardComponent->GetKeyID("Paused");
    homeDistanceKey = BlackboardComponent->GetKeyID("HomeDistance");
    groupLeadDistanceKey = BlackboardComponent->GetKeyID("GroupLeadDistance");

    homeLocation = possPawn->GetActorLocation();
    BlackboardComponent->SetValue<UBlackboardKeyType_Float>(targetDistanceKey, MAX_FLT);
    BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(targetPointLocationKey, homeLocation);
    BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(isPausedKey, false);

    SetCurrentAIStateBK(DefaultState);
    // Launch behavior Tree
    BehaviorTreeComponent->StartTree(*BehaviorTree);

    if (PerceptionComponent) {
        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AACFAIController::HandlePerceptionUpdated);
    }

    if (ThreatComponent) {
        ThreatComponent->OnNewMaxThreateningActor.AddDynamic(this, &AACFAIController::HandleMaxThreatUpdated);
    }

    EnableCharacterComponents(false);
}

void AACFAIController::OnUnPossess()
{
    Super::OnUnPossess();

    if (PerceptionComponent) {
        PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AACFAIController::HandlePerceptionUpdated);
    }

    if (ThreatComponent) {
        ThreatComponent->OnNewMaxThreateningActor.RemoveDynamic(this, &AACFAIController::HandleMaxThreatUpdated);
    }
}

void AACFAIController::EnableCharacterComponents(bool bEnabled)
{
    UACFInteractionComponent* interComponent = CharacterOwned->FindComponentByClass<UACFInteractionComponent>();

    if (interComponent) {
        if (bEnabled) {
            interComponent->RegisterComponent();
        } else {
            interComponent->UnregisterComponent();
        }
    }
}

void AACFAIController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!bIsAggressive) {
        return;
    }

    if (!Actor || Actor == GetPawn()) {
        return;
    }
    
    //Checking if our enemy is a group
    AACFAIGroupSpawner* spawner = Cast<AACFAIGroupSpawner>(Actor);
    if (spawner) {
        Actor = spawner->GetAgentNearestTo(CharacterOwned->GetActorLocation());
        if (!Actor || Actor == GetPawn()) {
            return;
        }
    }
    IACFEntityInterface* entity = Cast<IACFEntityInterface>(Actor);

    if (entity && IACFEntityInterface::Execute_IsEntityAlive(Actor) && UACFFunctionLibrary::AreEnemyTeams(GetWorld(), CombatTeam, IACFEntityInterface::Execute_GetEntityCombatTeam(Actor))) {
        //select if it's a newly discovered potential threat
     
        if (!ThreatComponent->IsThreatening(Actor) && ThreatComponent->IsActorAPotentialThreat(Actor)) {
            const float threat = ThreatComponent->GetDefaultThreatForActor(Actor);
            if (threat == 0.f) {
                return;
            }
            ThreatComponent->AddThreat(Actor, threat);
        }
    }
}

void AACFAIController::HandleMaxThreatUpdated(AActor* newMax)
{
    SetTarget(newMax);
}

void AACFAIController::BeginPlay()
{
    Super::BeginPlay();
    CombatBehaviorComponent = FindComponentByClass<UACFCombatBehaviourComponent>();
}

void AACFAIController::EndPlay(const EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    AACFGameState* gameState = GetWorld()->GetGameState<AACFGameState>();
    if (gameState) {
        gameState->RemoveAIFromBattle(this);
    }
}

void AACFAIController::TriggerCommand(const FGameplayTag& command)
{
    if (CommandsManagerComp) {
		CommandsManagerComp->TriggerCommand(command);
    }
}

class UACFGroupAIComponent* AACFAIController::GetOwningGroup_Implementation()
{
    return GroupOwner;
}

bool AACFAIController::IsPartOfGroup_Implementation()
{
    return GroupOwner != nullptr;
}

void AACFAIController::SetPatrolPath(AACFPatrolPath* inPatrol)
{
    patrolPath = inPatrol;
}

void AACFAIController::ResetToDefaultState()
{
    SetCurrentAIStateBK(DefaultState);
}

void AACFAIController::SetWaitDurationTimeBK(float time)
{
    if (Blackboard)
        Blackboard->SetValue<UBlackboardKeyType_Float>(commandDurationTimeKey, time);
}

float AACFAIController::GetCommandDurationTimeBK() const
{
    if (Blackboard)
        return Blackboard->GetValue<UBlackboardKeyType_Float>(commandDurationTimeKey);
    return -1.f;
}

void AACFAIController::SetLeadActorBK(AActor* target)
{
    BlackboardComponent->SetValue<UBlackboardKeyType_Object>(groupLeaderKey, target);
}

FVector AACFAIController::GetTargetPointLocationBK() const
{
    return BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(targetPointLocationKey);
}

bool AACFAIController::GetIsPausedBK() const
{
    return BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(isPausedKey);
}

AActor* AACFAIController::GetLeadActorBK() const
{
    if (Blackboard) {
        AActor* lead = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(groupLeaderKey));
        return lead;
    }
    return nullptr;
}

void AACFAIController::SetTargetActorBK(AActor* target)
{
    if (Blackboard)
        Blackboard->SetValue<UBlackboardKeyType_Object>(targetActorKey, target);
}

void AACFAIController::SetTargetLocationBK(const FVector& targetLocation)
{
    if (Blackboard)
        Blackboard->SetValue<UBlackboardKeyType_Vector>(targetPointLocationKey, targetLocation);
}

void AACFAIController::SetCurrentAIStateBK(EAIState aiState)
{
    if (GetAIStateBK() == EAIState::EBattle && CombatBehaviorComponent && PerceptionComponent && aiState != EAIState::EBattle) {
        CombatBehaviorComponent->UninitBehavior();
    }

    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Enum>(aiStateKey, static_cast<UBlackboardKeyType_Enum::FDataType>(aiState));
    }

    OnAIStateChanged.Broadcast(aiState);

    ELocomotionState* locstate = LocomotionStateByAIState.Find(aiState);
    if (locstate && CharacterOwned && CharacterOwned->GetACFCharacterMovementComponent()) {
        CharacterOwned->GetACFCharacterMovementComponent()->SetLocomotionState(*locstate);
    }

    if (aiState == EAIState::EFollowLeader) {
        SetFocus(GetLeadActorBK(), EAIFocusPriority::Gameplay);
    }

    if (aiState == EAIState::EBattle && CombatBehaviorComponent) {
        CombatBehaviorComponent->InitBehavior(this);
    }

    //GameState settings
    AACFGameState* gameState = GetWorld()->GetGameState<AACFGameState>();
    if (!gameState) {
        return;
    }

    if (aiState == EAIState::EBattle) {
        gameState->AddAIToBattle(this);
    } else {
        gameState->RemoveAIFromBattle(this);
    }
}

AActor* AACFAIController::GetTargetActorBK() const
{
    if (Blackboard) {
        AActor* targetActor = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(targetActorKey));
        return targetActor;
    }
    return nullptr;
}

EAIState AACFAIController::GetAIStateBK() const
{
    return static_cast<EAIState>(Blackboard->GetValue<UBlackboardKeyType_Enum>(aiStateKey));
}

EAICombatState AACFAIController::GetCombatStateBK() const
{
    return static_cast<EAICombatState>(Blackboard->GetValue<UBlackboardKeyType_Enum>(combatStateKey));
}

void AACFAIController::SetCombatStateBK(EAICombatState aiState)
{
    if (Blackboard)
        Blackboard->SetValue<UBlackboardKeyType_Enum>(combatStateKey, static_cast<UBlackboardKeyType_Enum::FDataType>(aiState));

    if (CombatBehaviorComponent && CharacterOwned) {
        const FAICombatStateConfig* locstate = CombatBehaviorComponent->CombatStatesConfig.FindByKey(aiState);
        if (locstate && CharacterOwned->GetACFCharacterMovementComponent()) {
            CharacterOwned->GetACFCharacterMovementComponent()->SetLocomotionState(locstate->LocomotionState);
        }
    }
    OnAICombatStateChanged.Broadcast(aiState);
}


float AACFAIController::GetTargetActorDistanceBK() const
{
    if (Blackboard) {
        return Blackboard->GetValue<UBlackboardKeyType_Float>(targetActorDistanceKey);
    }

    return -1.f;
}

float AACFAIController::GetTargetPointDistanceBK() const
{
    if (Blackboard) {
        return Blackboard->GetValue<UBlackboardKeyType_Float>(targetDistanceKey);
    }
    return -1.f;
}


float AACFAIController::GetLeadActorDistanceBK() const
{
    if (Blackboard) {
        return Blackboard->GetValue<UBlackboardKeyType_Float>(groupLeadDistanceKey);
    }
    return -1.f;
}

float AACFAIController::GetHomeDistanceBK() const
{
    if (Blackboard) {
        return Blackboard->GetValue<UBlackboardKeyType_Float>(homeDistanceKey);
    }
    return -1.f;
}

void AACFAIController::SetTargetPointDistanceBK(float distance)
{
    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Float>(targetDistanceKey, distance);
    }      
}



void AACFAIController::SetTargetActorDistanceBK(float distance)
{
    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Float>(targetActorDistanceKey, distance);
    }
}

void AACFAIController::SetIsPausedBK(bool isPaused)
{
    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Bool>(isPausedKey, isPaused);
    }
}

void AACFAIController::SetLeadActorDistanceBK(float distance)
{
    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Float>(groupLeadDistanceKey, distance);
    }
}

void AACFAIController::SetHomeDistanceBK(float distance)
{
    if (Blackboard) {
        Blackboard->SetValue<UBlackboardKeyType_Float>(homeDistanceKey, distance);
    }
}

float AACFAIController::GetPathDistanceFromTarget()
{
    return GetPathFollowingComponent()->GetRemainingPathCost();
}

bool AACFAIController::TryGoToNextWaypoint()
{
    if (!CharacterOwned) {
        return false;
    }
    if (!patrolPath) {
        TryGetPatrolPath();
        if (!patrolPath) {
            return false;
        }
    }

    const USplineComponent* path = patrolPath->GetSplineComponent();
    if (!path) {
        return false;
    }
    const FTransform waypoint = path->GetTransformAtSplinePoint(patrolIndex, ESplineCoordinateSpace::World);
    if (bIsReversed) {
        if (patrolIndex <= 0) {
            patrolIndex = 0;
            bIsReversed = false;
        } else {
            patrolIndex--;
        }
    } else {
        if (patrolIndex >= path->GetNumberOfSplinePoints() - 1) {
            if (!patrolPath->GetPatrolConfig().ReversePath) {
                patrolIndex = 0;
            } else {
                bIsReversed = true,
                patrolIndex = path->GetNumberOfSplinePoints() - 2;
            }
        } else {
            patrolIndex++;
        }
    }

    FVector outLoc;
    UNavigationSystemV1::K2_ProjectPointToNavigation(this, waypoint.GetLocation(), outLoc, nullptr, UNavigationQueryFilter::StaticClass());
    SetTargetLocationBK(outLoc);
    SetWaitDurationTimeBK(patrolPath->GetPatrolConfig().WaitTimeAtPatrolPoint);
    return true;
}

void AACFAIController::TryGetPatrolPath()
{
    if (GroupOwner) {
        SetPatrolPath(GroupOwner->GetPatrolPath());
    } else {
        const UACFAIPatrolComponent* patrolComp = CharacterOwned->FindComponentByClass<UACFAIPatrolComponent>();
        if (patrolComp) {
            SetPatrolPath(patrolComp->GetPathToFollow());
        }
    }
}

void AACFAIController::TeleportNearLead()
{
    const AActor* lead = GetLeadActorBK();
    if (lead) {
        const FVector homeLoc = lead->GetActorLocation();
        FVector outLoc;
        if (CharacterOwned && UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, homeLoc, outLoc, TeleportNearLeadRadius)) {
            CharacterOwned->SetActorLocation(homeLoc);
        }
    }
}

bool AACFAIController::TryUpdateLeadRef()
{
    if (IsPartOfGroup() && GetOwningGroup()) {
        AActor* lead = GetOwningGroup()->GetGroupLead();
        if (lead) {
            SetLeadActorBK(lead);
            return true;
        }
    }
    return false;
}

void AACFAIController::OnTargetDeathHandle()
{
    if (CharacterOwned->IsAlive()) {
        AActor* target = ThreatComponent->GetActorWithHigherThreat();
        if (target) {
            SetTarget(target);
        } else {
            UE_LOG(LogTemp, Log, TEXT("%s is requesting new target - AACFAIController::OnTargetDeathHandle"), *this->GetName());
            RequestAnotherTarget();
        }
    }
}

void AACFAIController::SetGroupOwner(class UACFGroupAIComponent* group, int32 groupIndex, bool disablePerception, bool bOverrideTeam)
{
    if (group) {
        GroupOwner = group;
        GroupIndex = groupIndex;

        UAIPerceptionComponent* perception = FindComponentByClass<UAIPerceptionComponent>();
        if (perception && disablePerception) {
            perception->UnregisterComponent();
        }
    }
}

void AACFAIController::SetTarget(AActor* inTarget)
{
    AActor* currentTarget = GetTargetActorBK();

    TargetingComponent->SetCurrentTarget(inTarget);

    if (!inTarget || inTarget == CharacterOwned) {
        ThreatComponent->RemoveThreatening(currentTarget);
        if (currentTarget) {

            UACFDamageHandlerComponent* oldDamageComp = currentTarget->FindComponentByClass<UACFDamageHandlerComponent>();
            if (oldDamageComp) {
                oldDamageComp->OnOwnerDeath.RemoveDynamic(this, &AACFAIController::OnTargetDeathHandle);
            }
        }
        return;
    }

    // if our target is changed to a new target
    if (inTarget != currentTarget || GetAIStateBK() != EAIState::EBattle) {
        IACFEntityInterface* entity = Cast<IACFEntityInterface>(inTarget);

        if (currentTarget) {

            UACFDamageHandlerComponent* oldDamageComp = currentTarget->FindComponentByClass<UACFDamageHandlerComponent>();
            if (oldDamageComp) {
                oldDamageComp->OnOwnerDeath.RemoveDynamic(this, &AACFAIController::OnTargetDeathHandle);
            }
        }

        if (entity && inTarget && IACFEntityInterface::Execute_IsEntityAlive(inTarget) && UACFFunctionLibrary::AreEnemyTeams(GetWorld(), CombatTeam, IACFEntityInterface::Execute_GetEntityCombatTeam(inTarget))) {
            SetTargetActorBK(inTarget);
            SetCurrentAIStateBK(EAIState::EBattle);
            UACFDamageHandlerComponent* damageComp = inTarget->FindComponentByClass<UACFDamageHandlerComponent>();
            if (damageComp) {
                damageComp->OnOwnerDeath.AddDynamic(this, &AACFAIController::OnTargetDeathHandle);
            }

            if (GroupOwner) {
                if (!GroupOwner->IsInBattle()) {
                    GroupOwner->SetInBattle(true, inTarget);
                }
            }
        }
    }
}

AActor* AACFAIController::GetTarget() const
{
    return TargetingComponent->GetCurrentTarget();
}

bool AACFAIController::HasTarget() const
{
    return TargetingComponent->HasTarget();
}

bool AACFAIController::RequestAnotherTarget()
{
    if (CharacterOwned && IsValid(CharacterOwned)) {
        AActor* target = ThreatComponent->GetActorWithHigherThreat();
        if (GroupOwner && !target) {
            target = GroupOwner->RequestNewTarget(this);
        }

        if (target) {
            SetTarget(target);
            return true;
        } 
    }
    return false;
}

void AACFAIController::HandleCharacterHit(const FACFDamageEvent& damageReceived)
{
    if (!damageReceived.DamageDealer) {
        return;
    }

    if (bShouldReactOnHit) {
        ThreatComponent->AddThreat(damageReceived.DamageDealer, damageReceived.FinalDamage);
        if (GetAIStateBK() != EAIState::EBattle) {
            SetTarget(ThreatComponent->GetActorWithHigherThreat());
        }

        if (GroupOwner && !GroupOwner->IsInBattle()) {
            GroupOwner->SetInBattle(true, damageReceived.DamageDealer);
        }
    }
}

void AACFAIController::HandleCharacterDeath()
{
    SetCurrentAIStateBK(EAIState::EFollowLeader);
    UnPossess();
    Destroy();
}
