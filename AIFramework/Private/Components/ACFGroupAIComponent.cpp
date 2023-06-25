// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFGroupAIComponent.h"
#include "ACFAIController.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFThreatManagerComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFPlayerController.h"
#include "Game/ACFTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include <Engine/World.h>
#include <GameFramework/Pawn.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFGroupAIComponent::UACFGroupAIComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bInBattle = false;
    SetIsReplicatedByDefault(true);
    SpawnPoints.Add(FVector(150.f, 150.f, 0.f));
    SpawnPoints.Add(FVector(150.f, -150.f, 0.f));
    SpawnPoints.Add(FVector(-150.f, -150.f, 0.f));
    SpawnPoints.Add(FVector(-150.f, 150.f, 0.f));
}

void UACFGroupAIComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFGroupAIComponent, groupLead);
    DOREPLIFETIME(UACFGroupAIComponent, bInBattle);
    DOREPLIFETIME(UACFGroupAIComponent, AICharactersInfo);
}
// Called when the game starts
void UACFGroupAIComponent::BeginPlay()
{
    Super::BeginPlay();
    SetReferences();
}

void UACFGroupAIComponent::SetReferences()
{
    groupLead = Cast<AActor>(GetOwner());
}

void UACFGroupAIComponent::OnComponentLoaded_Implementation()
{
    for (int32 index = 0; index < AICharactersInfo.Num(); index++) {
        FAIAgentsInfo& agent = AICharactersInfo[index];
        TArray<AActor*> foundActors;
        UGameplayStatics::GetAllActorsOfClassWithTag(this, agent.characterClass, FName(*agent.Guid), foundActors);
        if (foundActors.Num() == 0) {
            UE_LOG(LogTemp, Error, TEXT("Impossible to find actor"));
            continue;
        }
        agent.AICharacter = Cast<AACFCharacter>(foundActors[0]);
        agent.AIController = Cast<AACFAIController>(agent.AICharacter->GetController());
        InitAgent(agent, index);
    }
}

void UACFGroupAIComponent::SendCommandToCompanions_Implementation(FGameplayTag command)
{
    Internal_SendCommandToAgents(command);
}

bool UACFGroupAIComponent::SendCommandToCompanions_Validate(FGameplayTag command)
{
    return true;
}

void UACFGroupAIComponent::SpawnGroup_Implementation()
{
    if (bAlreadySpawned && !bCanSpawnMultitpleTimes) {
        return;
    }
    Internal_SpawnGroup();
    bAlreadySpawned = true;
}

bool UACFGroupAIComponent::SpawnGroup_Validate()
{
    return true;
}

void UACFGroupAIComponent::DespawnGroup_Implementation(const bool bResetSpawnArray /*= false*/)
{
    if (!bResetSpawnArray) {
        AIToSpawn.Empty();
    }
    if (!bResetSpawnArray) {
        for (FAIAgentsInfo& agent : AICharactersInfo) {
            if (agent.AICharacter && agent.AICharacter->IsAlive()) {
                const TSubclassOf<AACFCharacter> charClass = agent.AICharacter->GetClass();
                AddAIToSpawn(charClass);
                agent.AICharacter->KillCharacter();
            }
        }
    }
    OnAllAgentDeath.Broadcast();
    AICharactersInfo.Empty();
    bAlreadySpawned = false;
}

bool UACFGroupAIComponent::DespawnGroup_Validate(const bool bResetSpawnArray /*= false*/)
{
    return true;
}

void UACFGroupAIComponent::InitAgents()
{
    for (int32 index = 0; index < AICharactersInfo.Num(); index++) {
        if (AICharactersInfo.IsValidIndex(index)) {
            InitAgent(AICharactersInfo[index], index);
        }
    }
}

void UACFGroupAIComponent::InitAgent(FAIAgentsInfo& agent, int32 childIndex)
{
    if (!agent.AICharacter) {
        ensure(false);
        return;
    }

    if (!agent.AICharacter->GetController()) {
        agent.AICharacter->SpawnDefaultController();
    }
    agent.AIController = Cast<AACFAIController>(agent.AICharacter->GetController());

    if (agent.AIController && agent.AICharacter) {
        if (!groupLead) {
            SetReferences();
        }
        agent.AIController->SetLeadActorBK(groupLead);
        agent.AIController->SetDefaultState(DefaultAiState);
        agent.AIController->SetCurrentAIStateBK(DefaultAiState);
        agent.AIController->SetPatrolPath(PatrolPath);
        if (bOverrideAgentTeam) {
            agent.AICharacter->AssignTeam(CombatTeam);
        }
        agent.characterClass = (agent.AICharacter->GetClass());
        check(agent.characterClass);
        if (!agent.AICharacter->Tags.Contains(FName(*agent.Guid))) {
            const FString newGuid = FGuid::NewGuid().ToString();
            agent.AICharacter->Tags.Add(FName(*newGuid));
            agent.Guid = newGuid;
        }
        agent.AIController->SetGroupOwner(this, childIndex, bOverrideAgentPerception, bOverrideAgentTeam);
        if (!agent.AICharacter->OnDeath.IsAlreadyBound(this, &UACFGroupAIComponent::HandleAgentDeath)) {
            agent.AICharacter->OnDeath.AddDynamic(this, &UACFGroupAIComponent::HandleAgentDeath);
        }
    }
}

void UACFGroupAIComponent::AddAIToSpawn(const TSubclassOf<AACFCharacter>& charClass)
{
    if (AIToSpawn.Contains(charClass)) {
        AIToSpawn.FindByKey(charClass)->TotalNumber++;
    } else {
        AIToSpawn.Add(FAISpawnInfo(charClass));
    }
}

void UACFGroupAIComponent::RemoveAIToSpawn(const TSubclassOf<AACFCharacter>& charClass)
{
    if (AIToSpawn.Contains(charClass)) {
        const int32 aiNum = AIToSpawn.FindByKey(charClass)->TotalNumber--;
        if (aiNum == 0) {
            AIToSpawn.Remove(charClass);
        }
    }
}

void UACFGroupAIComponent::ReplaceAIToSpawn(const TArray<FAISpawnInfo>& newAIs)
{
    AIToSpawn.Empty();
    AIToSpawn = newAIs;
}

bool UACFGroupAIComponent::GetAgentByIndex(int32 index, FAIAgentsInfo& outAgent) const
{
    if (AICharactersInfo.IsValidIndex(index)) {
        outAgent = AICharactersInfo[index];
        return true;
    }
    return false;
}

void UACFGroupAIComponent::Internal_SendCommandToAgents(FGameplayTag command)
{
    for (FAIAgentsInfo& achar : AICharactersInfo) {
        if (achar.AIController) {
            achar.AIController->TriggerCommand(command);
        } else {

            ensure(false);
        }
    }
}

void UACFGroupAIComponent::SetEnemyGroup(UACFGroupAIComponent* inEnemyGroup)
{
    if (inEnemyGroup && UACFFunctionLibrary::AreEnemyTeams(GetWorld(), GetCombatTeam(), inEnemyGroup->GetCombatTeam())) {
        enemyGroup = inEnemyGroup;
    }
}

void UACFGroupAIComponent::HandleAgentDeath(class AACFCharacter* agent) {
    OnChildDeath(agent);
}

FVector UACFGroupAIComponent::GetGroupCentroid() const
{
    TArray<AActor*> actors;
    for (const auto& agent : AICharactersInfo) {
        if (agent.AICharacter) {
            actors.Add(agent.AICharacter);
        }
    }
    return UGameplayStatics::GetActorArrayAverageLocation(actors);
}

class AACFCharacter* UACFGroupAIComponent::RequestNewTarget(const AACFAIController* requestSender)
{
    // First Try to help lead
    const AACFCharacter* lead = Cast<AACFCharacter>(requestSender->GetLeadActorBK());
    if (lead) {
        const AACFCharacter* newTarget = Cast<AACFCharacter>(lead->GetTarget());
        if (newTarget && newTarget->IsMyEnemy(requestSender->GetBaseAICharacter()))
            return Cast<AACFCharacter>(lead->GetTarget());
    }

    // Then Try to help other in  the group
    if (AICharactersInfo.IsValidIndex(0) && AICharactersInfo[0].AICharacter->IsValidLowLevel() && AICharactersInfo[0].AIController->IsValidLowLevel()) {
        for (FAIAgentsInfo achar : AICharactersInfo) {
            if (achar.AIController && achar.AIController != requestSender) {
                AACFCharacter* newTarget = Cast<AACFCharacter>(achar.AIController->GetTargetActorBK());
                if (newTarget && newTarget->IsAlive() && achar.AIController->GetAIStateBK() == EAIState::EBattle) {
                    return newTarget;
                }
            }
        }
    }

    // Then Try to help other in  the group
    if (enemyGroup) {
        return enemyGroup->GetAgentNearestTo(requestSender->GetPawn()->GetActorLocation());
    }

    return nullptr;
}

void UACFGroupAIComponent::Internal_SpawnGroup()
{
    if (AIToSpawn.Num() > 0) {
        const UWorld* world = GetWorld();
        if (world) {
            FActorSpawnParameters spawnParam;
            spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            int32 currentIndex = 0;
            for (auto& aiSpawn : AIToSpawn) {
                const TSubclassOf<AACFCharacter> ClassToSpawn = aiSpawn.AIClassBP;

                for (int32 index = 0; index < aiSpawn.TotalNumber; index++) {
                    const int32 childGroupIndex = AddAgentToGroup(ClassToSpawn, aiSpawn.SpawnRotation);
                }
            }
        }
        bAlreadySpawned = true;
        OnAgentsSpawned.Broadcast();
    } else {
        UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
    }
}

uint8 UACFGroupAIComponent::AddAgentToGroup(const TSubclassOf<AACFCharacter>& ClassToSpawn, const EAgentRotation rotation)
{
    UWorld* const world = GetWorld();

    ensure(GetOwner()->HasAuthority());

    if (!world)
        return -1;

    if (!groupLead) {
        SetReferences();
        if (!groupLead) {
            return -1;
        }
    }

    if (AICharactersInfo.Num() >= MaxSimultaneousAgents) {
        return -1;
    }

    FAIAgentsInfo newCharacterInfo;

    const int32 localGroupIndex = AICharactersInfo.Num();
    const FVector myLocation = groupLead->GetActorLocation();
    FVector additivePos = FVector::ZeroVector;
    if (SpawnPoints.IsValidIndex(localGroupIndex)) {
        additivePos = SpawnPoints[localGroupIndex];
    } else {
        additivePos.X = UKismetMathLibrary::RandomFloatInRange(-DefaultSpawnOffset.X, DefaultSpawnOffset.X);
        additivePos.Y = UKismetMathLibrary::RandomFloatInRange(-DefaultSpawnOffset.Y, DefaultSpawnOffset.Y);
    }
    const FVector spawnLocation = myLocation + additivePos;
    FTransform spawnTransform;
    spawnTransform.SetLocation(spawnLocation);

    switch (rotation) {
    case EAgentRotation::EGroupForward:
        spawnTransform.SetRotation(groupLead->GetActorRotation().Quaternion());
        break;
    case EAgentRotation::EGroupCenter:
        spawnTransform.SetRotation(UKismetMathLibrary::FindLookAtRotation(spawnLocation, myLocation).Quaternion());
        break;
    }

    newCharacterInfo.AICharacter = world->SpawnActorDeferred<AACFCharacter>(
            ClassToSpawn, spawnTransform, nullptr, nullptr,
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (newCharacterInfo.AICharacter) {

        UGameplayStatics::FinishSpawningActor(newCharacterInfo.AICharacter, spawnTransform);

        // End Spawn
        if (!newCharacterInfo.AICharacter->GetController()) {
            newCharacterInfo.AICharacter->SpawnDefaultController();
        }

        newCharacterInfo.AIController = Cast<AACFAIController>(newCharacterInfo.AICharacter->GetController());
        if (newCharacterInfo.AIController) {
            InitAgent(newCharacterInfo, localGroupIndex);
        } else {
            UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
        }
        AICharactersInfo.Add(newCharacterInfo);
        return localGroupIndex;
    }
    return 0;
}

bool UACFGroupAIComponent::AddExistingCharacterToGroup(AACFCharacter* character)
{
    const UWorld* world = GetWorld();

    if (!world) {
        return false;
    }

    if (!groupLead) {
        SetReferences();
    }

    if (AICharactersInfo.Contains(character)) {
        InitAgents();
        return true;
    }

    FAIAgentsInfo newCharacterInfo;
    newCharacterInfo.AICharacter = character;

    if (newCharacterInfo.AICharacter) {
        if (!newCharacterInfo.AICharacter->GetController()) {
            newCharacterInfo.AICharacter->SpawnDefaultController();
        }

        uint8 childIndex = AICharactersInfo.Num();
        newCharacterInfo.AIController = Cast<AACFAIController>(newCharacterInfo.AICharacter->GetController());
        if (newCharacterInfo.AIController) {
            InitAgent(newCharacterInfo, childIndex);
        } else {
            UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
        }

        AICharactersInfo.Add(newCharacterInfo);
        return true;
    }
    return false;
}

void UACFGroupAIComponent::ReInitAgent(AACFCharacter* character)
{
    if (AICharactersInfo.Contains(character)) {
        FAIAgentsInfo* newCharacterInfo = AICharactersInfo.FindByKey(character);
        const int32 index = AICharactersInfo.IndexOfByKey(character);
        InitAgent(*newCharacterInfo, index);
    }
}

AACFCharacter* UACFGroupAIComponent::GetAgentNearestTo(const FVector& location) const
{
    AACFCharacter* bestAgent = nullptr;
    float minDistance = 999999.f;
    for (FAIAgentsInfo achar : AICharactersInfo) {
        if (achar.AICharacter && achar.AICharacter->IsAlive()) {
            const float distance = FVector::Distance(location, achar.AICharacter->GetActorLocation());
            if (distance <= minDistance) {
                minDistance = distance;
                bestAgent = achar.AICharacter;
            }
        }
    }
    return bestAgent;
}

bool UACFGroupAIComponent::RemoveAgentFromGroup(AACFCharacter* character)
{
    if (!character) {
        return false;
    }

    AACFAIController* contr = Cast<AACFAIController>(character->GetController());
    if (!contr) {
        return false;
    }

    const FAIAgentsInfo agentInfo(character, contr);

    if (AICharactersInfo.Contains(agentInfo)) {
        AICharactersInfo.RemoveSingle(agentInfo);
        return true;
    }

    return false;
}

void UACFGroupAIComponent::SetInBattle(bool inBattle, AActor* newTarget)
{
    bInBattle = inBattle;
    if (bInBattle) {
        const APawn* aiTarget = Cast<APawn>(newTarget);
        if (aiTarget) {

            // Check if target is part of a group
            AController* targetCont = aiTarget->GetController();
            if (targetCont) {
                const bool bImplements = targetCont->GetClass()->ImplementsInterface(UACFGroupAgentInterface::StaticClass());
                if (bImplements && IACFGroupAgentInterface::Execute_IsPartOfGroup(targetCont)) {
                    UACFGroupAIComponent* groupComp = IACFGroupAgentInterface::Execute_GetOwningGroup(targetCont);
                    SetEnemyGroup(groupComp);
                } else {
                    enemyGroup = nullptr;
                }
            }
        }

        int32 index = 0;
        for (const FAIAgentsInfo& achar : AICharactersInfo) {
            if (!achar.AIController || achar.AIController->GetAIStateBK() == EAIState::EBattle || !achar.AICharacter->IsAlive()) {
                continue;
            }

            // Trying to assign to every agent in the group that is not in battle an enemy in the enemy group
            AActor* nextTarget = newTarget;
            FAIAgentsInfo adversary;
            if (enemyGroup && enemyGroup->GetGroupSize() > 0) {
                if (achar.AIController && !achar.AIController->HasTarget()) {
                    if (enemyGroup->GetGroupSize() > index) {
                        enemyGroup->GetAgentByIndex(index, adversary);
                        index++;
                    } else {
                        index = 0;
                        enemyGroup->GetAgentByIndex(index, adversary);
                        index++;
                    }
                    nextTarget = adversary.AICharacter;
                }
            }
            UACFThreatManagerComponent* threatComp = achar.AIController->GetThreatManager();
            if (nextTarget) {
                const float newThreat = threatComp->GetDefaultThreatForActor(nextTarget);
                if (newThreat > 0.f) {
                    // if the enemy we found is valid, we add aggro for that enemy
                    threatComp->AddThreat(nextTarget, newThreat + 10.f);
                } else {
                    // otherwise we go in the new target
                    threatComp->AddThreat(newTarget, threatComp->GetDefaultThreatForActor(newTarget));
                }
            }
        }
    }
}

void UACFGroupAIComponent::OnChildDeath(const AACFCharacter* character)
{   
    const int32 index = AICharactersInfo.IndexOfByKey(character);
    if (AICharactersInfo.IsValidIndex(index)) {
        AICharactersInfo.RemoveAt(index);
    }
    OnAgentDeath.Broadcast(character);
    if (AICharactersInfo.Num() == 0) {
        OnAllAgentDeath.Broadcast();
    }

}
