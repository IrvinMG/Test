// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFActionTypes.h"
#include "Animation/AnimMontage.h"
#include "Components/ACFActionsManagerComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameplayTagContainer.h>

#include "ACFBaseAction.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class ACTIONSSYSTEM_API UACFBaseAction : public UObject {
    GENERATED_BODY()

    friend UACFActionsManagerComponent;

public:
    UACFBaseAction();
    /*Blueprint callable function that will get us the cooldown remaining for the action*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetCooldownTimeRemaining();

protected:
    /*Called when the action is successfully triggered*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionStarted(const FString& contextString = "");
    virtual void OnActionStarted_Implementation(const FString& contextString = "");

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void ClientsOnActionStarted(const FString& contextString = "");
    virtual void ClientsOnActionStarted_Implementation(const FString& contextString = "");

    /*Called when the action is finished. Normally if BindActionToAnimation is set to true
        this is called when the animation is finished*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionEnded();
    virtual void OnActionEnded_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void ClientsOnActionEnded();
    virtual void ClientsOnActionEnded_Implementation();

    /*Called when the action is successfully triggered and BEFORE the OnActionStarted.
        If the character is coming from the DefaultState previousState will be nullptr*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionTransition(class UACFBaseAction* previousState);
    virtual void OnActionTransition_Implementation(class UACFBaseAction* previousState);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void PlayEffects();
    virtual void PlayEffects_Implementation();

    /*Called every frame if the ActionsManagerComponent of this character has bCanTick set to true*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnTick(float DeltaTime);
    virtual void OnTick_Implementation(float DeltaTime);

    /*Used to implement your own activation condition for the execution of this action. */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    bool CanExecuteAction(class ACharacter* owner = nullptr);
    virtual bool CanExecuteAction_Implementation(class ACharacter* owner = nullptr);

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetActionTag() const
    {
        return ActionTag;
    }

    /*Implement this to select the name of the montage section that should be played when executing this action */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    FName GetMontageSectionName();
    virtual FName GetMontageSectionName_Implementation();

    /*Implement this to configure the warp info. Only called if MontageReproductionType is set to Warp */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void GetWarpInfo(FACFWarpReproductionInfo& outWarpInfo);
    virtual void GetWarpInfo_Implementation(FACFWarpReproductionInfo& outWarpInfo);

    /*Implement this if you only want to configure the transform of the warp point.
    Only called if MontageReproductionType is set to Warp */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    FTransform GetWarpTransform();
    virtual FTransform GetWarpTransform_Implementation();

    /*Implement this if you only want to configure the transform of the warp point.
    Only called if MontageReproductionType is set to Warp */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    class USceneComponent* GetWarpTargetComponent();
    virtual class USceneComponent* GetWarpTargetComponent_Implementation();

    /*Implement this to select the name of the montage section that should be played when executing this action */
    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetMontageInfo(const FACFMontageInfo& montageInfo);
         
    /*Interrupt current action AND animation Immediately*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void StopActionImmediately();

    /*To trigger a Notable point you have to add an ACFNotifyAction to your Animation Montage.
        Called during the execution of the action when the montage reaches the ACFNotifyAction*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnNotablePointReached();
    virtual void OnNotablePointReached_Implementation();

     /*Clients Version for notable point*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void ClientsOnNotablePointReached();
    virtual void ClientsOnNotablePointReached_Implementation();

    /*To trigger a SubActionState you have to place a ACFActionSubState notify in your Animation Montage.
        Called during the execution of the action when the montage reaches the beginning of the Substate*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnSubActionStateEntered();
    virtual void OnSubActionStateEntered_Implementation();

    /*To trigger a SubActionState you have to place a ACFActionSubState notify in your Animation Montage.
        Called during the execution of the action when the montage reaches the end  of the Substate*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnSubActionStateExited();
    virtual void OnSubActionStateExited_Implementation();

    /*Clients version for OnSubActionState*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void ClientsOnSubActionStateEntered();
    virtual void ClientsOnSubActionStateEntered_Implementation();

    /*Clients version for OnSubActionState*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void ClientsOnSubActionStateExited();
    virtual void ClientsOnSubActionStateExited_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    float GetPlayRate();
    virtual float GetPlayRate_Implementation();

    /*Play the animation montage related to this action. Called automatically by default*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void ExecuteAction();

    bool bBindActionToAnimation = true;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetMontageReproductionType(EMontageReproductionType reproType);

    /*Called to force the end of the action. Does not stop the animation but allows the trigger of eventually stored
    actions or set the actual state to the default one*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void ExitAction();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FActionConfig ActionConfig;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFActionsManagerComponent>ActionsManager;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr < class ACharacter> CharacterOwner;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr <class UAnimMontage> animMontage;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    FACFMontageInfo MontageInfo;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FGameplayTag ActionTag;

    bool bIsExecutingAction = false;

    virtual void Internal_OnActivated(class UACFActionsManagerComponent* actionmanger, class UAnimMontage* inAnimMontage, const FString& contextString);

    virtual void Internal_OnDeactivated();

    void PrepareMontageInfo();

    UWorld* GetWorld() const override { return CharacterOwner ? CharacterOwner->GetWorld() : nullptr; }

    /*A reference to the cooldown started for the Action*/
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FTimerHandle CooldownTimerReference;
    void BindAnimationEvents();

private:
    bool GetTerminated() const { return bTerminated; }
    void SetTerminated(bool val) { bTerminated = val; }

    UFUNCTION()
    void HandleMontageFinished(UAnimMontage* _animMontage, bool _bInterruptted);

    UFUNCTION()
    void HandleMontageStarted(UAnimMontage* _animMontage);

    void UnbinAnimationEvents();

    bool bIsInSubState = false;

    UPROPERTY()
    TObjectPtr <class UARSStatisticsComponent> StatisticComp;

    bool bTerminated = false;
};
