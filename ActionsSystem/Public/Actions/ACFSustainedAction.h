// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved.

#pragma once

#include "Actions/ACFBaseAction.h"
#include "CoreMinimal.h"

#include "ACFSustainedAction.generated.h"


/**
 *
 */
UCLASS()
class ACTIONSSYSTEM_API UACFSustainedAction : public UACFBaseAction {
    GENERATED_BODY()

public:
    UACFSustainedAction();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ReleaseAction();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void PlayActionSection(FName sectionName);

protected:
    UPROPERTY(EditDefaultsOnly, Category = ACF)
    FName FinalSectionName = "End";

    /*Called when release action is triggered during the execution*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionReleased(float elapsedTimeSeconds);
    virtual void OnActionReleased_Implementation(float elapsedTimeSeconds);

     float startTime;

    virtual void Internal_OnActivated(class UACFActionsManagerComponent* actionmanger, 
        class UAnimMontage* inAnimMontage, const FString& contextString) override;
};
