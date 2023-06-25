// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2023. All Rights Reserved.

#include "Actions/ACFBaseAction.h"
#include "ACMEffectsDispatcherComponent.h"
#include "AIController.h"
#include "ARSStatisticsComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <TimerManager.h>
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void UACFBaseAction::OnActionStarted_Implementation(const FString& contextString)
{
}

void UACFBaseAction::ClientsOnActionStarted_Implementation(const FString& contextString /*= ""*/)
{
}

void UACFBaseAction::OnActionEnded_Implementation()
{
}

void UACFBaseAction::ClientsOnActionEnded_Implementation()
{
}

void UACFBaseAction::OnActionTransition_Implementation(class UACFBaseAction* previousState)
{
}

void UACFBaseAction::PlayEffects_Implementation()
{
    if (ActionConfig.ActionEffect.ActionParticle || ActionConfig.ActionEffect.NiagaraParticle || ActionConfig.ActionEffect.ActionSound) {

        AGameStateBase* gameState = UGameplayStatics::GetGameState(CharacterOwner);

        if (gameState) {
            UACMEffectsDispatcherComponent* EffectComp = gameState->FindComponentByClass<UACMEffectsDispatcherComponent>();
            EffectComp->PlayActionEffectLocally(ActionConfig.ActionEffect, CharacterOwner);
            return;
        }
    }
}

void UACFBaseAction::OnTick_Implementation(float DeltaTime)
{
}

bool UACFBaseAction::CanExecuteAction_Implementation(class ACharacter* owner)
{
    return true;
}

FName UACFBaseAction::GetMontageSectionName_Implementation()
{
    return NAME_None;
}

void UACFBaseAction::GetWarpInfo_Implementation(FACFWarpReproductionInfo& outWarpInfo)
{
    outWarpInfo.WarpConfig = ActionConfig.WarpInfo;
    //  float sectionStart, sectionEnd;
    const FName sectionName = GetMontageSectionName();
    int32 currentIndex = animMontage->GetSectionIndex(sectionName);
    if (currentIndex < 0) {
        currentIndex = 0;
    }
    float endTime;
    animMontage->GetSectionStartAndEndTime(currentIndex, outWarpInfo.WarpConfig.WarpStartTime, endTime);
    outWarpInfo.WarpConfig.WarpEndTime = outWarpInfo.WarpConfig.WarpStartTime + ActionConfig.WarpInfo.WarpEndTime;
    if (ActionConfig.WarpInfo.TargetType == EWarpTargetType::ETargetTransform) {
        const FTransform endTransform = GetWarpTransform();
        FVector localScale;
        UKismetMathLibrary::BreakTransform(endTransform, outWarpInfo.WarpLocation, outWarpInfo.WarpRotation, localScale);
    } else if (ActionConfig.WarpInfo.TargetType == EWarpTargetType::ETargetTransform) {
        outWarpInfo.TargetComponent = GetWarpTargetComponent();
    }
}

FTransform UACFBaseAction::GetWarpTransform_Implementation()
{
    ensure(false);
    return FTransform();
}

class USceneComponent* UACFBaseAction::GetWarpTargetComponent_Implementation()
{
    ensure(false);
    return nullptr;
}

void UACFBaseAction::SetMontageInfo(const FACFMontageInfo& montageInfo)
{
}

void UACFBaseAction::StopActionImmediately()
{
    if (ActionsManager) {
        ActionsManager->StopActionImmeditaley();
    }
}

void UACFBaseAction::OnNotablePointReached_Implementation()
{
}

void UACFBaseAction::ClientsOnNotablePointReached_Implementation()
{
}

void UACFBaseAction::OnSubActionStateEntered_Implementation()
{
}

void UACFBaseAction::OnSubActionStateExited_Implementation()
{
}
void UACFBaseAction::HandleMontageStarted(UAnimMontage* inAnimMontage)
{
    UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());

    UnbinAnimationEvents();

    if (animMontage == inAnimMontage) {
        animinst->OnMontageBlendingOut.AddDynamic(this, &UACFBaseAction::HandleMontageFinished);
    }
}

void UACFBaseAction::UnbinAnimationEvents()
{
    UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());
    if (animinst) {
        animinst->OnMontageStarted.RemoveDynamic(this, &UACFBaseAction::HandleMontageStarted);
        animinst->OnMontageBlendingOut.RemoveDynamic(this, &UACFBaseAction::HandleMontageFinished);
    }
}

void UACFBaseAction::SetMontageReproductionType(EMontageReproductionType reproType)
{
    ActionConfig.MontageReproductionType = reproType;
}

void UACFBaseAction::PrepareMontageInfo()
{

    MontageInfo.MontageAction = animMontage;
    MontageInfo.ReproductionSpeed = GetPlayRate();

    if (ActionConfig.bPlayRandomMontageSection) {
        const int32 numSections = animMontage->CompositeSections.Num();

        const int32 sectionToPlay = FMath::RandHelper(numSections);

        MontageInfo.StartSectionName = animMontage->GetSectionName(sectionToPlay);
    } else {
        MontageInfo.StartSectionName = GetMontageSectionName();
    }
    MontageInfo.ReproductionType = ActionConfig.MontageReproductionType;
    MontageInfo.RootMotionScale = 1.f;
    FACFWarpReproductionInfo outWarp;

    UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();

    switch (MontageInfo.ReproductionType) {
    case EMontageReproductionType::ERootMotionScaled:
        MontageInfo.RootMotionScale = ActionConfig.RootMotionScale;
        break;
    case EMontageReproductionType::ERootMotion:
        break;
    case EMontageReproductionType::EMotionWarped:
        if (motionComp) {
            GetWarpInfo(outWarp);
            MontageInfo.WarpInfo = outWarp;
        }
        break;
    }
}
void UACFBaseAction::ExecuteAction()
{

    if (animMontage && ActionsManager) {
        if (bBindActionToAnimation) {
            BindAnimationEvents();
        }
        PrepareMontageInfo();
        ActionsManager->PlayReplicatedMontage(MontageInfo);
        bIsExecutingAction = true;
    } else {
        ExitAction();
    }
}

void UACFBaseAction::BindAnimationEvents()
{
    if (CharacterOwner) {
        UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());
        if (animinst) {
            animinst->OnMontageStarted.AddDynamic(this, &UACFBaseAction::HandleMontageStarted);
        }
    }
}

void UACFBaseAction::ExitAction()
{
    if (ActionsManager) {
        ActionsManager->InternalExitAction();
    }
}

void UACFBaseAction::ClientsOnSubActionStateEntered_Implementation()
{
}

void UACFBaseAction::ClientsOnSubActionStateExited_Implementation()
{
}

float UACFBaseAction::GetPlayRate_Implementation()
{
    return 1.f;
}

void UACFBaseAction::HandleMontageFinished(UAnimMontage* inAnimMontage, bool _bInterruptted)
{
    if (animMontage == inAnimMontage && bIsExecutingAction && !_bInterruptted) {
        ExitAction();
    }
}

void UACFBaseAction::Internal_OnActivated(class UACFActionsManagerComponent* actionManger, class UAnimMontage* inAnimMontage, const FString& contextString)
{
    ActionsManager = actionManger;
    animMontage = inAnimMontage;
    if (ActionsManager) {
        CharacterOwner = ActionsManager->CharacterOwner;
        ActionTag = ActionsManager->CurrentActionTag;
        StatisticComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();

        if (StatisticComp) {
            StatisticComp->ConsumeStatistics(ActionConfig.ActionCost);
            StatisticComp->AddAttributeSetModifier(ActionConfig.AttributeModifier);
        } else {
            UE_LOG(LogTemp, Warning, TEXT("No Statistic Component!"));
            ExitAction();
            return;
        }
    }

    if (ActionConfig.bStopBehavioralThree) {
        const AAIController* contr = Cast<AAIController>(CharacterOwner->GetController());
        if (contr) {
            UBehaviorTreeComponent* behavComp = contr->FindComponentByClass<UBehaviorTreeComponent>();
            if (behavComp) {
                behavComp->PauseLogic("Blocking Action");
            }
        }
    }

    OnActionStarted(contextString);

    if (ActionConfig.bAutoExecute) {
        ExecuteAction();
    }
}

void UACFBaseAction::Internal_OnDeactivated()
{
    if (bIsExecutingAction) {
        bIsExecutingAction = false;
    }
    UMotionWarpingComponent* motionComp = CharacterOwner->FindComponentByClass<UMotionWarpingComponent>();

    if (motionComp) {
        motionComp->RemoveWarpTarget(MontageInfo.WarpInfo.WarpConfig.SyncPoint);
    }

    UnbinAnimationEvents();

    if (StatisticComp) {
        StatisticComp->RemoveAttributeSetModifier(ActionConfig.AttributeModifier);
    }

    if (bIsInSubState) {
        OnSubActionStateExited();
    }

    // reset warp info
    if (ActionConfig.bStopBehavioralThree) {
        const AAIController* aiContr = Cast<AAIController>(CharacterOwner->GetController());
        if (aiContr) {
            UBehaviorTreeComponent* behavComp = aiContr->FindComponentByClass<UBehaviorTreeComponent>();
            if (behavComp) {
                behavComp->ResumeLogic("Blocking Action");
            }
        }
    }
    OnActionEnded();
}

UACFBaseAction::UACFBaseAction()
{
    bBindActionToAnimation = true;
}

float UACFBaseAction::GetCooldownTimeRemaining()
{
    UWorld* world = GetWorld();
    if (world) {
        return world->GetTimerManager().GetTimerRemaining(CooldownTimerReference);

    } else {
        return 0.0f;
    }
}
