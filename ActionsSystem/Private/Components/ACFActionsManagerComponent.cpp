// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL
// 2021. All Rights Reserved.

#include "Components/ACFActionsManagerComponent.h"
#include "ACFActionTypes.h"
#include "ACFActionsFunctionLibrary.h"
#include "ARSStatisticsComponent.h"
#include "ARSTypes.h"
#include "Actions/ACFBaseAction.h"
#include "Actions/ACFSustainedAction.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionWarpingComponent.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"
#include "RootMotionModifier.h"
#include "RootMotionModifier_SkewWarp.h"
#include <Engine/Engine.h>
#include <Engine/World.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameplayTagsManager.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>

// Sets default values for this component's properties
UACFActionsManagerComponent::UACFActionsManagerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked
    // every frame.  You can turn these features off to improve performance if you
    // don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    SetComponentTickEnabled(false);
    ActionsSet = UACFActionsSet::StaticClass();
    CurrentPriority = -1;
}

// Called when the game starts
void UACFActionsManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (ActionsSet) {
        ActionsSetInst = NewObject<UACFActionsSet>(this, ActionsSet);
    } else {
        UE_LOG(LogTemp, Error, TEXT("Invalid ActionSet Class- ActionsManager"));
    }

    MovesetsActionsInst.Empty();
    for (const auto& actionssetclass : MovesetActions) {
        if (actionssetclass.ActionsSet) {
            UACFActionsSet* moveset = NewObject<UACFActionsSet>(this, actionssetclass.ActionsSet);
            MovesetsActionsInst.Add(actionssetclass.TagName, moveset);
        } else {
            UE_LOG(LogTemp, Error, TEXT("Invalid ActionSet Class- ActionsManager"));
        }
    }
    CurrentPriority = -1;
    StoredAction = FGameplayTag();
    CharacterOwner = Cast<ACharacter>(GetOwner());
    if (CharacterOwner) {
        animInst = CharacterOwner->GetMesh()->GetAnimInstance();
        StatisticComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
        if (!StatisticComp) {
            UE_LOG(LogTemp, Warning, TEXT("No Statistiscs Component - ActionsManager"));
        }
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Character - ActionsManager"));
    }
    SetComponentTickEnabled(bCanTick);
    // DefaultActionState = UACFActionsFunctionLibrary::GetDefaultActionsState();
    StoredAction = FGameplayTag();
    CurrentActionTag = FGameplayTag();
}

void UACFActionsManagerComponent::StopActionImmeditaley_Implementation()
{

    Internal_StopCurrentAnimation();
    ClientsStopActionImmeditaley();
    ExitAction();
}

void UACFActionsManagerComponent::Internal_StopCurrentAnimation()
{
    FActionState action;
    if (GetActionByTag(CurrentActionTag, action)) {
        animInst->Montage_Stop(0.0f, action.MontageAction);
    }
}

void UACFActionsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //

    DOREPLIFETIME(UACFActionsManagerComponent, MontageInfo);
    DOREPLIFETIME(UACFActionsManagerComponent, CurrentActionTag);
    DOREPLIFETIME(UACFActionsManagerComponent, CurrentPriority);
    DOREPLIFETIME(UACFActionsManagerComponent, bIsPerformingAction);
    DOREPLIFETIME(UACFActionsManagerComponent, currentMovesetActionsTag);
}

// Called every frame
void UACFActionsManagerComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsPerformingAction && PerformingAction) {
        PerformingAction->OnTick(DeltaTime);
    }
}

void UACFActionsManagerComponent::TriggerActionByName(
    FName ActionTagName,
    EActionPriority Priority /*= EActionPriority::ELow*/, bool bCanBeStored /*= false*/, const FString& contextString)
{
    const FGameplayTag tag = UGameplayTagsManager::Get().RequestGameplayTag(ActionTagName);

    if (tag.IsValid()) {
        TriggerAction(tag, Priority, bCanBeStored, contextString);
    }
}

void UACFActionsManagerComponent::SetMovesetActions_Implementation(const FGameplayTag& movesetActionsTag)
{
    currentMovesetActionsTag = movesetActionsTag;
}

void UACFActionsManagerComponent::TriggerAction_Implementation(FGameplayTag ActionState,
    EActionPriority Priority /*= EActionPriority::ELow*/, bool bCanBeStored /*= false*/, const FString& contextString)
{
    if (!CharacterOwner) {
        return;
    }

    OnActionTriggered.Broadcast(ActionState, Priority);

    FActionState action;
    if (GetActionByTag(ActionState, action) && action.Action && CanExecuteAction(ActionState)) {
        if ((((int32)Priority > CurrentPriority)) || Priority == EActionPriority::EHighest) {
            LaunchAction(ActionState, Priority, contextString);
        } else if (CurrentActionTag != FGameplayTag() && bCanStoreAction && bCanBeStored) {
            StoreAction(ActionState);
        }
    } else {
        UE_LOG(LogTemp, Warning,
            TEXT("Invalid Action Configuration - ActionsManager"));
    }
}

void UACFActionsManagerComponent::PlayReplicatedMontage_Implementation(const FACFMontageInfo& montageInfo)
{
    MontageInfo = montageInfo;
    ClientPlayMontage(montageInfo);
}

bool UACFActionsManagerComponent::PlayReplicatedMontage_Validate(const FACFMontageInfo& montageInfo)
{
    return true;
}

void UACFActionsManagerComponent::ClientPlayMontage_Implementation(const FACFMontageInfo& montageInfo)
{
    MontageInfo = montageInfo;
    PlayCurrentMontage();
}

bool UACFActionsManagerComponent::IsActionOnCooldown(
    FGameplayTag action) const
{
    return onCooldownActions.Contains(action);
}

void UACFActionsManagerComponent::StoreAction(FGameplayTag ActionState)
{
    StoredAction = ActionState;
}

void UACFActionsManagerComponent::LaunchAction(const FGameplayTag& ActionState,
    const EActionPriority priority, const FString& contextString)
{
    FActionState action;
    if (GetActionByTag(ActionState, action) && action.Action) {
        if (PerformingAction) {
            action.Action->OnActionTransition(PerformingAction);
            TerminateCurrentAction();
        }
        PerformingAction = action.Action;
        CurrentActionTag = ActionState;
        bIsPerformingAction = true;
        PerformingAction->SetTerminated(false);
        PerformingAction->Internal_OnActivated(this, action.MontageAction, contextString);
        ClientsReceiveActionStarted(ActionState, contextString);
        CurrentPriority = (int32)priority;
        if (PerformingAction && PerformingAction->ActionConfig.bPlayEffectOnActionStart) {
            PerformingAction->PlayEffects();
        }
    }
}

void UACFActionsManagerComponent::SetCurrentAction(
    const FGameplayTag& ActionState)
{
    CurrentActionTag = ActionState;
}

void UACFActionsManagerComponent::TerminateCurrentAction()
{
    if (bIsPerformingAction && PerformingAction && !PerformingAction->GetTerminated()) {
        PerformingAction->Internal_OnDeactivated();
        PerformingAction->SetTerminated(true);
        PerformingAction = nullptr;
        ClientsReceiveActionEnded(CurrentActionTag);
        CurrentActionTag = FGameplayTag();
        CurrentPriority = -1;
    }
    bIsPerformingAction = false;
}

void UACFActionsManagerComponent::ClientsReceiveActionEnded_Implementation(
    const FGameplayTag& ActionState)
{
    PrintStateDebugInfo(false);
    FActionState action;
    if (GetActionByTag(ActionState, action) && action.Action) {

        action.Action->ClientsOnActionEnded();
    }
    OnActionFinished.Broadcast(ActionState);
}

void UACFActionsManagerComponent::ClientsStopActionImmeditaley_Implementation()
{
    Internal_StopCurrentAnimation();
}

void UACFActionsManagerComponent::ClientsReceiveActionStarted_Implementation(
    const FGameplayTag& ActionState, const FString& contextString)
{
    SetCurrentAction(ActionState);
    OnActionStarted.Broadcast(ActionState);
    PrintStateDebugInfo(true);

    FActionState action;
    if (GetActionByTag(ActionState, action) && action.Action) {
        StartCooldown(ActionState, action);
        action.Action->CharacterOwner = CharacterOwner;
        action.Action->ClientsOnActionStarted(contextString);
    }
}

bool UACFActionsManagerComponent::CanExecuteAction(FGameplayTag ActionState) const
{
    FActionState action;
    if (GetActionByTag(ActionState, action) && action.Action && StatisticComp) {
        if (CharacterOwner->GetCharacterMovement()->IsFalling() && !action.Action->ActionConfig.bPerformableInAir) {
            UE_LOG(LogTemp, Warning, TEXT("Actions Can't be exectuted while in air!"));
            return false;
        }

        if (StatisticComp->CheckCosts(action.Action->ActionConfig.ActionCost) && StatisticComp->CheckPrimaryAttributesRequirements(action.Action->ActionConfig.Requirements) && !IsActionOnCooldown(ActionState) && !bIsLocked && action.Action->CanExecuteAction(CharacterOwner) && StatisticComp->GetCurrentLevel() >= action.Action->ActionConfig.RequiredLevel) {
            UE_LOG(LogTemp, Warning, TEXT("Actions Costs OR Actions Attribute Requirements are not verified"));
            return true;
        }
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Actions Conditions are not verified"));
    }
    return false;
}

void UACFActionsManagerComponent::InternalExitAction()
{
    if (bIsPerformingAction && PerformingAction) {
        TerminateCurrentAction();
        if (StoredAction != FGameplayTag()) {
            TriggerAction(StoredAction);
            StoreAction(FGameplayTag());
        } else {
            SetCurrentAction(FGameplayTag());
            ClientsReceiveActionStarted(FGameplayTag(), "");
            PerformingAction = nullptr;
        }
    }
}

void UACFActionsManagerComponent::ExitAction()
{
    InternalExitAction();
}

void UACFActionsManagerComponent::ReleaseSustainedAction_Implementation(FGameplayTag actionTag)
{
    if (PerformingAction && PerformingAction->GetActionTag() == actionTag) {
        UACFSustainedAction* sustAction = Cast<UACFSustainedAction>(PerformingAction);
        if (sustAction) {
            sustAction->ReleaseAction();
        }
    }
}

void UACFActionsManagerComponent::PlayMontageSectionFromAction_Implementation(FGameplayTag actionTag, FName montageSection)
{
    if (PerformingAction && PerformingAction->GetActionTag() == actionTag) {
        UACFSustainedAction* sustAction = Cast<UACFSustainedAction>(PerformingAction);
        if (sustAction) {
            sustAction->PlayActionSection(montageSection);
        }
    }
}

void UACFActionsManagerComponent::FreeAction()
{
    CurrentPriority = -1;

    if (StoredAction != FGameplayTag()) {
        TriggerAction(StoredAction, EActionPriority::ELow);
        StoreAction(FGameplayTag());
    }
}

bool UACFActionsManagerComponent::GetMovesetActionByTag(const FGameplayTag& action, const FGameplayTag& Moveset, FActionState& outAction) const
{
    if (MovesetsActionsInst.Contains(Moveset)) {
        UACFActionsSet* actionSet = MovesetsActionsInst.FindChecked(Moveset);
        if (actionSet) {
            return actionSet->GetActionByTag(action, outAction);
        }
    }
    return false;
}

bool UACFActionsManagerComponent::GetCommonActionByTag(const FGameplayTag& action, FActionState& outAction) const
{
    if (ActionsSetInst) {
        return ActionsSetInst->GetActionByTag(action, outAction);
    }
    return false;
}

void UACFActionsManagerComponent::AddOrModifyAction(const FActionState& action)
{
    if (ActionsSetInst) {
        return ActionsSetInst->AddOrModifyAction(action);
    }
}

FGameplayTag UACFActionsManagerComponent::GetCurrentActionTag() const
{
    return CurrentActionTag;
}

bool UACFActionsManagerComponent::GetActionByTag(const FGameplayTag& Action, FActionState& outAction) const
{
    if (ActionsSetInst) {
        if (GetMovesetActionByTag(Action, currentMovesetActionsTag, outAction)) {
            return true;
        } else if (GetCommonActionByTag(Action, outAction)) {
            return true;
        }
    } else {
        return false;
    }
    return false;
}

void UACFActionsManagerComponent::PlayCurrentActionFX()
{
    if (PerformingAction) {
        PerformingAction->PlayEffects();
    }
}

bool UACFActionsManagerComponent::IsInActionSubstate() const
{
    return PerformingAction && PerformingAction->bIsInSubState;
}

void UACFActionsManagerComponent::AnimationsReachedNotablePoint()
{
    if (bIsPerformingAction && PerformingAction && PerformingAction->bIsExecutingAction && CharacterOwner) {
        if (CharacterOwner->HasAuthority()) {
            PerformingAction->OnNotablePointReached();
            PerformingAction->ClientsOnNotablePointReached();
        } else {
            PerformingAction->ClientsOnNotablePointReached();
        }
    }
}

void UACFActionsManagerComponent::StartSubState()
{
    if (bIsPerformingAction && PerformingAction && PerformingAction->bIsExecutingAction && CharacterOwner) {
        PerformingAction->bIsInSubState = true;
        if (CharacterOwner->HasAuthority()) {
            PerformingAction->OnSubActionStateEntered();
            PerformingAction->ClientsOnSubActionStateEntered();
        } else {
            PerformingAction->ClientsOnSubActionStateEntered();
        }
    }
}

void UACFActionsManagerComponent::EndSubState()
{
    if (PerformingAction && PerformingAction->bIsExecutingAction && CharacterOwner) {
        PerformingAction->bIsInSubState = false;
        if (CharacterOwner->HasAuthority()) {
            PerformingAction->OnSubActionStateExited();
            PerformingAction->ClientsOnSubActionStateExited();
        } else {
            PerformingAction->ClientsOnSubActionStateExited();
        }
    }
}

void UACFActionsManagerComponent::PrintStateDebugInfo(bool bIsEntring)
{
    if (bPrintDebugInfo && GEngine && CharacterOwner) {
        FString ActionName;
        CurrentActionTag.GetTagName().ToString(ActionName);
        FString MessageToPrint;
        if (bIsEntring) {
            MessageToPrint = CharacterOwner->GetName() + FString(" Entered State:") + ActionName;
        } else {
            MessageToPrint = CharacterOwner->GetName() + FString(" Exited State:") + ActionName;
        }

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, MessageToPrint,
            false);
    }
}

void UACFActionsManagerComponent::PlayCurrentMontage()
{
    if (MontageInfo.MontageAction && CharacterOwner) {
        CharacterOwner->SetAnimRootMotionTranslationScale(1.f);
        UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();
        if (motionComp) {
            motionComp->RemoveWarpTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint);
        }
        switch (MontageInfo.ReproductionType) {
        case EMontageReproductionType::ERootMotionScaled:
            CharacterOwner->SetAnimRootMotionTranslationScale(MontageInfo.RootMotionScale);
            break;
        case EMontageReproductionType::ERootMotion:
            break;
        case EMontageReproductionType::EMotionWarped:
            PrepareWarp();
            break;
        }

        CharacterOwner->PlayAnimMontage(MontageInfo.MontageAction, MontageInfo.ReproductionSpeed, MontageInfo.StartSectionName);
    }
}

void UACFActionsManagerComponent::PrepareWarp()
{
    UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();
    const FTransform targetTransform = FTransform(MontageInfo.WarpInfo.WarpRotation, MontageInfo.WarpInfo.WarpLocation);

    if (motionComp && MontageInfo.WarpInfo.WarpConfig.bAutoWarp) {

        FMotionWarpingTarget targetPoint;
        if (MontageInfo.WarpInfo.WarpConfig.TargetType == EWarpTargetType::ETargetTransform) {
            targetPoint = FMotionWarpingTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint, targetTransform);
        } else if (MontageInfo.WarpInfo.WarpConfig.TargetType == EWarpTargetType::ETargetComponent) {
            targetPoint = FMotionWarpingTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint, MontageInfo.WarpInfo.TargetComponent, NAME_None, MontageInfo.WarpInfo.WarpConfig.bMagneticFollow);
        }
        motionComp->AddOrUpdateWarpTarget(targetPoint);
        /*UE 5.1 UPDATE UNCOMMENT ME
        FMotionWarpingTarget targetPoint;
        if (MontageInfo.WarpInfo.WarpConfig.TargetType == EWarpTargetType::ETargetTransform) {
            targetPoint = FMotionWarpingTarget(targetTransform);
        } else if (MontageInfo.WarpInfo.WarpConfig.TargetType == EWarpTargetType::ETargetComponent) {
            targetPoint = FMotionWarpingTarget(MontageInfo.WarpInfo.TargetComponent, NAME_None, MontageInfo.WarpInfo.WarpConfig.bMagneticFollow);
        }
        motionComp->AddOrUpdateWarpTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint, targetPoint);

          */

        URootMotionModifier_SkewWarp::AddRootMotionModifierSkewWarp(motionComp, MontageInfo.MontageAction, MontageInfo.WarpInfo.WarpConfig.WarpStartTime,
            MontageInfo.WarpInfo.WarpConfig.WarpEndTime, MontageInfo.WarpInfo.WarpConfig.SyncPoint, EWarpPointAnimProvider::None, targetTransform, NAME_None, true, true, true,
            MontageInfo.WarpInfo.WarpConfig.RotationType, MontageInfo.WarpInfo.WarpConfig.WarpRotationTime);
        if (bPrintDebugInfo) {
            UKismetSystemLibrary::DrawDebugSphere(this, MontageInfo.WarpInfo.WarpLocation, 100.f, 12, FLinearColor::Red, 5.f);
        }
    } else {
        motionComp->RemoveWarpTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint);
    }
}

void UACFActionsManagerComponent::StartCooldown(const FGameplayTag& action,
    FActionState& actionState)
{
    if (actionState.Action->ActionConfig.CoolDownTime == 0.f)
        return;

    FTimerDelegate TimerDel;
    FTimerHandle TimerHandle;
    TimerDel.BindUFunction(this, FName("OnCooldownFinished"), action);

    UWorld* world = GetWorld();
    if (world) {
        onCooldownActions.Add(action);
        world->GetTimerManager().SetTimer(
            TimerHandle, TimerDel, actionState.Action->ActionConfig.CoolDownTime,
            false);
        actionState.Action->CooldownTimerReference = TimerHandle;
    }
}

void UACFActionsManagerComponent::OnRep_MontageInfo()
{
    // PlayCurrentMontage();
}

void UACFActionsManagerComponent::OnCooldownFinished(
    const FGameplayTag& action)
{
    if (onCooldownActions.Contains(action)) {
        onCooldownActions.Remove(action);
    }
}
