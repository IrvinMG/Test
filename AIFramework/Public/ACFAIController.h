// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFAITypes.h"
#include "ACFBaseAIController.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "CoreMinimal.h"
#include "Game/ACFDamageType.h"
#include "Groups/ACFGroupAgentInterface.h"
#include <DetourCrowdAIController.h>
#include <GameplayTagContainer.h>
#include <Perception/AIPerceptionTypes.h>
#include "ACFCCTypes.h"
#include <BehaviorTree/Blackboard/BlackboardKey.h>

#include "ACFAIController.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChanged, const EAIState, AIState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAICombatStateChanged, const EAICombatState, AICombatState);

UCLASS()
class AIFRAMEWORK_API AACFAIController : public AACFBaseAIController, public IACFGroupAgentInterface {
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type reason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UBlackboardComponent> BlackboardComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFCombatBehaviourComponent> CombatBehaviorComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFCommandsManagerComponent> CommandsManagerComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UATSAITargetComponent> TargetingComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFThreatManagerComponent> ThreatComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF ")
    TObjectPtr<class UBehaviorTree> BehaviorTree;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr <class AACFCharacter> CharacterOwned;

    /*Default AI state for this agent*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | AI State")
    EAIState DefaultState;

    /*Indicates the default locomotion state of this AI for each AI state*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | AI State")
    TMap<EAIState, ELocomotionState> LocomotionStateByAIState;

    /*If set to true this AI will try to come back to the Lead Position once it is over MaxDistanceFromHome*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Home")
    bool bBoundToHome = false;

    /*If bBoundToHome  is true, once the AI is over this distance from his lead/home position, he will try to come back home*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bBoundToHome), BlueprintReadOnly, Category = "ACF | Home")
    float MaxDistanceFromHome = 5000.f;

    /*If set to true this AI will be teleported back near the Lead Position once it is over TeleportHomeTriggerDistance*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Lead")
    bool bTeleportToLead = false;

    /*Distance to trigger the teleport*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bTeleportToLead), BlueprintReadOnly, Category = "ACF | Lead")
    float TeleportToLeadTriggerDistance = 8500.f;

    /*Radius in which the controlled pawn is spawned near the lead*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bTeleportToLead), BlueprintReadOnly, Category = "ACF | Lead")
    float TeleportNearLeadRadius = 2500.f;

    /*Distance for which the controlled pawn stop fighting against his target*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Combat")
    float LoseTargetDistance = 3500.f;

    /*Indicates if this agent attacks an enemy on perception*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Combat")
    bool bIsAggressive = true;

    /*Indicates if this agent  should react once he gets hit*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Combat")
    bool bShouldReactOnHit = true;

    
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    int32 GroupIndex;

public:
    AACFAIController(const FObjectInitializer& ObjectInitializer);

    /// <summary>
    /// /Groupable Entity Interface
    /// </summary>
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACF | Group")
    class UACFGroupAIComponent* GetOwningGroup();
    virtual class UACFGroupAIComponent* GetOwningGroup_Implementation() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACF | Group")
    bool IsPartOfGroup();
    virtual bool IsPartOfGroup_Implementation() override;
    /// <summary>
    /// /End Groupable Entity
    /// </summary>
    /// 


private:
    void EnableCharacterComponents(bool bEnabled);

    UPROPERTY()
    TObjectPtr<class UACFGroupAIComponent> GroupOwner;

    int32 patrolIndex = 0;

    UPROPERTY()
    TObjectPtr<class AACFPatrolPath> patrolPath;

    FVector homeLocation;

    bool bIsReversed = false;

    struct FBlackboard::FKey commandDurationTimeKey,
        targetActorKey,
        targetActorDistanceKey,
        targetPointLocationKey,
        aiStateKey,
        groupLeaderKey, targetDistanceKey,
        combatStateKey, isPausedKey,groupLeadDistanceKey, homeDistanceKey;

    UFUNCTION()
    void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION()
    void HandleMaxThreatUpdated(AActor* newMax);

    UFUNCTION()
    void HandleCharacterHit(const FACFDamageEvent& damageReceived);

    UFUNCTION()
    void HandleCharacterDeath();

    UFUNCTION()
    virtual void OnTargetDeathHandle();

    void TryGetPatrolPath();

    virtual void OnPossess(APawn* _possPawn) override;

    virtual void OnUnPossess() override;

public:
    friend class AACFGroupAIController;

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class AACFCharacter* GetBaseAICharacter() const { return CharacterOwned; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UACFCombatBehaviourComponent* GetCombatBehavior() const { return CombatBehaviorComponent; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UACFCommandsManagerComponent* GetCommandManager() const { return CommandsManagerComp; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UACFThreatManagerComponent* GetThreatManager() const { return ThreatComponent; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE float GetMaxDistanceFromHome() const { return MaxDistanceFromHome; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE float GetTeleportLeadDistanceTrigger() const { return TeleportToLeadTriggerDistance; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE float GetLoseTargetDistance() const { return LoseTargetDistance; }

    struct FBlackboard::FKey GetCommandDurationTimeKey() const { return commandDurationTimeKey; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE int32 GetIndexInGroup() const { return GroupIndex; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UBehaviorTreeComponent* GetBehaviorThreeComponent() const { return BehaviorTreeComponent; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE class UACFGroupAIComponent* GetGroup() const { return GroupOwner; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE bool ShouldReturnHome() const { return bBoundToHome; }

    UFUNCTION(BlueprintPure, Category = "ACF | Getter")
    FORCEINLINE bool ShouldTeleportNearLead() const { return bTeleportToLead; }

    UFUNCTION(BlueprintCallable, Category = "ACF | State")
    void SetDefaultState(EAIState inDefaultState)
    {
        DefaultState = inDefaultState;
    }

    UFUNCTION(BlueprintCallable, Category = "ACF | State")
    void ResetToDefaultState();

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetWaitDurationTimeBK(float time);

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    float GetCommandDurationTimeBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    EAICombatState GetCombatStateBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    EAIState GetAIStateBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    AActor* GetTargetActorBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    FVector GetTargetPointLocationBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    bool GetIsPausedBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    class AActor* GetLeadActorBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    float GetTargetActorDistanceBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    float GetTargetPointDistanceBK() const;
  
    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    float GetLeadActorDistanceBK() const;

    UFUNCTION(BlueprintPure, Category = "ACF | Blackboard")
    float GetHomeDistanceBK() const;

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetTargetLocationBK(const FVector& targetLocation);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetLeadActorBK(AActor* leadActor);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetTargetActorBK(AActor* target);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetCurrentAIStateBK(EAIState aiState);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetCombatStateBK(EAICombatState combatState);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetTargetPointDistanceBK(float distance);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetTargetActorDistanceBK(float distance);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetIsPausedBK(bool isPaused);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetLeadActorDistanceBK(float distance);

    UFUNCTION(BlueprintCallable, Category = "ACF | Blackboard")
    void SetHomeDistanceBK(float distance);

  
    UFUNCTION(BlueprintCallable, Category = "ACF | Lead")
    void TeleportNearLead();

    UFUNCTION(BlueprintCallable, Category = "ACF | Lead")
    bool TryUpdateLeadRef();

    UFUNCTION(BlueprintPure, Category = "ACF | Home")
    FVector GetHomeLocation() const { return homeLocation; }

    UFUNCTION(BlueprintCallable, Category = "ACF | Patrol")
    bool TryGoToNextWaypoint();

    UFUNCTION(BlueprintCallable, Category = "ACF | Patrol")
    float GetPathDistanceFromTarget();

    UFUNCTION(BlueprintCallable, Category = "ACF | Patrol")
    void SetPatrolPath(class AACFPatrolPath* inPatrol);

    UFUNCTION(BlueprintPure, Category = "ACF | Patrol")
    class AACFPatrolPath* GetPatrolPath() const {
        return patrolPath;
    } 

    /*Forces the AI to target the provided actor*/
    UFUNCTION(BlueprintCallable, Category = "ACF | Target")
    void SetTarget(AActor* target);

    /*Returns the actor targeted by this AI as set in the ATSTargetComponent*/
    UFUNCTION(BlueprintPure, Category = "ACF | Target")
    AActor* GetTarget() const;

    /*Trues if this AI has a valid target*/
    UFUNCTION(BlueprintPure, Category = "ACF | Target")
    bool HasTarget() const;

    /*Tries to request another target, returns true if a new target is found*/
    UFUNCTION(BlueprintCallable, Category = "ACF | Target")
    bool RequestAnotherTarget();

    /*Executes the provided command from the ACFCommandMenagerComponent*/
    UFUNCTION(BlueprintCallable, Category = "ACF | Command")
    void TriggerCommand(const FGameplayTag& command);

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAIStateChanged OnAIStateChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAICombatStateChanged OnAICombatStateChanged;

    void SetGroupOwner(class UACFGroupAIComponent* group, int32 groupIndex, bool disablePerception, bool bOverrideTeam);
};
