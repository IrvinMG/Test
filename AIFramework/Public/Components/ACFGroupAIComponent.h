// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFAITypes.h"
#include "Actors/ACFCharacter.h"
#include "CoreMinimal.h"
#include <Components/SceneComponent.h>
#include <Components/SplineComponent.h>
#include <GameplayTagContainer.h>

#include "ACFGroupAIComponent.generated.h"

struct FAISpawnInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentDeath, const AACFCharacter*, character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAgentDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAgentsSpawned);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class AIFRAMEWORK_API UACFGroupAIComponent : public USceneComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFGroupAIComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void SetReferences();

protected:
    /*Should use group or agent perception?*/
    UPROPERTY(EditAnywhere, Category = "ACF | AI Config")
    bool bOverrideAgentPerception = true;

    UPROPERTY(EditAnywhere, Category = "ACF | AI Config")
    bool bOverrideAgentTeam = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | AI Config")
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | AI Config")
    EAIState DefaultAiState = EAIState::EPatrol;

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = "ACF | Spawn")
    TArray<FAISpawnInfo> AIToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Spawn")
    uint8 MaxSimultaneousAgents = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Spawn")
    bool bCanSpawnMultitpleTimes = false;

    UPROPERTY(EditAnywhere, Category = Category = "ACF | Spawn")
    FVector2D DefaultSpawnOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = "ACF | Spawn")
    TArray<FVector> SpawnPoints;

    UPROPERTY(Replicated, Replicated, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class AActor> groupLead;

    UPROPERTY(SaveGame, Replicated)
    TArray<FAIAgentsInfo> AICharactersInfo;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnComponentLoaded();

    UPROPERTY(BlueprintReadOnly, Replicated, Category = ACF)
    bool bInBattle;

public:
    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAgentDeath OnAgentDeath;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAllAgentDeath OnAllAgentDeath;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAgentsSpawned OnAgentsSpawned;

    UFUNCTION()
    virtual void OnChildDeath(const AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    class AACFCharacter* GetAgentNearestTo(const FVector& location) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsInBattle() const { return bInBattle; }

    UFUNCTION(BlueprintPure, Category = ACF)
    ETeam GetCombatTeam() const
    {
        return CombatTeam;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE int32 GetGroupSize() const { return AICharactersInfo.Num(); }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool RemoveAgentFromGroup(AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void GetGroupAgents(TArray<FAIAgentsInfo>& outAgents) const { outAgents = AICharactersInfo; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetInBattle(bool inBattle, AActor* newTarget);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void AddAIToSpawn(const TSubclassOf<AACFCharacter>& charClass);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void RemoveAIToSpawn(const TSubclassOf<AACFCharacter>& charClass);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ReplaceAIToSpawn(const TArray<FAISpawnInfo>& newAIs);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetAgentByIndex(int32 index, FAIAgentsInfo& outAgent) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetPatrolPath(class AACFPatrolPath* inPatrol)
    {
        PatrolPath = inPatrol;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    class AACFPatrolPath* GetPatrolPath() const
    {
        return PatrolPath;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFGroupAIComponent* GetEnemyGroup() const { return enemyGroup; }

    UFUNCTION(BlueprintPure, Category = ACF)
    class AActor* GetGroupLead() const { return groupLead; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector GetGroupCentroid() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    class AACFCharacter* RequestNewTarget(const AACFAIController* requestSender);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void SendCommandToCompanions(FGameplayTag command);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void SpawnGroup();

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void DespawnGroup(const bool bResetSpawnArray = false);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool AddExistingCharacterToGroup(AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ReInitAgent(AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool IsAlreadyInGroup(const AACFCharacter* character) const
    {
        return AICharactersInfo.Contains(character);
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool CanSpawnMultitpleTimes() const
    {
        return bCanSpawnMultitpleTimes;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetCanSpawnMultitpleTimes(bool bEnabled)
    {
        bCanSpawnMultitpleTimes = bEnabled;
    }

    void InitAgents();

private:
    UPROPERTY(SaveGame)
    bool bAlreadySpawned = false;

    void Internal_SendCommandToAgents(FGameplayTag command);

    UPROPERTY()
    TObjectPtr<class UACFGroupAIComponent> enemyGroup;

    UPROPERTY()
    TArray<int32> FormationInfo;

    UPROPERTY()
    TObjectPtr<class AACFPatrolPath> PatrolPath;

    void Internal_SpawnGroup();

    uint8 AddAgentToGroup(const TSubclassOf<AACFCharacter>& ClassToSpawn, const EAgentRotation rotation);
    void InitAgent(FAIAgentsInfo& agent, int32 childIndex);

    void SetEnemyGroup(UACFGroupAIComponent* inEnemyGroup);

    UFUNCTION()
    void HandleAgentDeath(class AACFCharacter* agent);
};
