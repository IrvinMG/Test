// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFActionTypes.h"
#include "ARSStatisticsComponent.h"
#include "ARSTypes.h"
#include "Actions/ACFActionsSet.h"
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>

#include "ACFActionsManagerComponent.generated.h"

class UACFBaseAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStarted, FGameplayTag, ActionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnded, FGameplayTag, ActionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionTriggered, FGameplayTag, ActionState, EActionPriority, Priority);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSSYSTEM_API UACFActionsManagerComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFActionsManagerComponent();

protected:
    friend class UACFBaseAction;
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class ACharacter* CharacterOwner;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
    bool bCanTick = true;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
    bool bPrintDebugInfo = false;

    /*The Actions set to be used for this character*/
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
    TSubclassOf<UACFActionsSet> ActionsSet;

    /*The ActionsSet to be used when a specific moveset is applied*/
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (TitleProperty = "TagName"), Category = ACF)
    TArray<FActionsSet> MovesetActions;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr < UACFActionsSet> ActionsSetInst = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TMap<FGameplayTag, TObjectPtr<UACFActionsSet>> MovesetsActionsInst;

 

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void TriggerActionByName(FName ActionTagName, EActionPriority Priority = EActionPriority::ELow, bool bCanBeStored = false, const FString& contextString = "");

    UFUNCTION(BlueprintCallable, Category = ACF)
    void LockActionsTrigger()
    {
        bIsLocked = true;
        TerminateCurrentAction();
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void UnlockActionsTrigger() { bIsLocked = false; }

    /*Tries to trigger the action with the provided tag. It's priority will be used to eventually interrupt lower priority actions.
    I bCanBeStored is set to true if the action can't be played immediately, it will be stored and retriggered at the end of current action.
    Context String will be sent to the OnActionStarted event and can be used to pass generic data tot he action n */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void TriggerAction(FGameplayTag ActionState, EActionPriority Priority = EActionPriority::ELow, bool bCanBeStored = false, const FString& contextString = "");

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SetMovesetActions(const FGameplayTag& movesetActionsTag);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentMovesetActionsTag() const { return currentMovesetActionsTag; }

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void PlayReplicatedMontage(const FACFMontageInfo& montageInfo);

    UFUNCTION(NetMulticast, Reliable)
    void ClientPlayMontage(const FACFMontageInfo& montageInfo);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StopStoringActions()
    {
        bCanStoreAction = false;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StartStoringActions()
    {
        bCanStoreAction = true;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool IsActionOnCooldown(FGameplayTag action) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void StoreAction(FGameplayTag Action);

    UFUNCTION(Blueprintpure, Category = ACF)
    FORCEINLINE FGameplayTag GetStoredAction() const { return StoredAction; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool CanExecuteAction(FGameplayTag Action) const;

    /*Terminates current action*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void ExitAction();

    /*If current action state is "actionTag", plays his final montage section
    and terminates the action*/
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void ReleaseSustainedAction(FGameplayTag actionTag);

    /*If current action state is "actionTag", plays "montageSection" of it
    without terminating it*/
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void PlayMontageSectionFromAction(FGameplayTag actionTag, FName montageSection);

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnActionStarted OnActionStarted;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnActionEnded OnActionFinished;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnActionTriggered OnActionTriggered;

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetCurrentActionTag() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFBaseAction* GetCurrentAction() const
    {
        return PerformingAction;
    }

   /*Immediately interrupts the animation and exit current action*/
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void StopActionImmeditaley();

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetActionByTag(const FGameplayTag& Action, FActionState& outAction) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void PlayCurrentActionFX();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsInActionState(FGameplayTag state) const { return CurrentActionTag == state; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsPerformingAction() const { return bIsPerformingAction; }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsInActionSubstate() const;

    void AnimationsReachedNotablePoint();

    void StartSubState();
    void EndSubState();
    void FreeAction();

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetMovesetActionByTag(const FGameplayTag& action, const FGameplayTag& Moveset, FActionState& outAction) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetCommonActionByTag(const FGameplayTag& action, FActionState& outAction) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void AddOrModifyAction(const FActionState& action);

private:
    void InternalExitAction();

    void LaunchAction(const FGameplayTag& ActionState, const EActionPriority priority, const FString& contextString = "");

    void SetCurrentAction(const FGameplayTag& state);

    void TerminateCurrentAction();

    UPROPERTY()
    class UAnimInstance* animInst;

    UFUNCTION(NetMulticast, Reliable)
    void ClientsReceiveActionStarted(const FGameplayTag& ActionState, const FString& contextString);

    UFUNCTION(NetMulticast, Reliable)
    void ClientsReceiveActionEnded(const FGameplayTag& ActionState);

    UFUNCTION(NetMulticast, Reliable)
    void ClientsStopActionImmeditaley();


    UPROPERTY(Replicated)
    bool bIsPerformingAction;

    TObjectPtr<UACFBaseAction> PerformingAction;

    UPROPERTY()
    FActionState CurrentActionState;

    UPROPERTY(Replicated)
    FGameplayTag CurrentActionTag;

    UPROPERTY()
    FGameplayTag StoredAction;

    UPROPERTY(Replicated)
    int32 CurrentPriority;

    UPROPERTY(Replicated)
    FGameplayTag currentMovesetActionsTag;

    UPROPERTY()
    bool bCanStoreAction = true;

    UPROPERTY()
    class UARSStatisticsComponent* StatisticComp;

    void PrintStateDebugInfo(bool bIsEntring);

    void PlayCurrentMontage();

    void PrepareWarp();

    UPROPERTY()
    TArray<FGameplayTag> onCooldownActions;

    void StartCooldown(const FGameplayTag& action, FActionState& actionState);

    UPROPERTY(ReplicatedUsing = OnRep_MontageInfo)
    FACFMontageInfo MontageInfo;

    UFUNCTION()
    void OnRep_MontageInfo();

    UFUNCTION()
    void OnCooldownFinished(const FGameplayTag& action);

    void Internal_StopCurrentAnimation();

    bool bIsLocked = false;
};
