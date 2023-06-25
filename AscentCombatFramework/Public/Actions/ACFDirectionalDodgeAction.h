// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "Actions/ACFBaseAction.h"
#include "CoreMinimal.h"

#include "ACFDirectionalDodgeAction.generated.h"

/**
 *
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFDirectionalDodgeAction : public UACFBaseAction {
    GENERATED_BODY()

protected:
    UACFDirectionalDodgeAction();

    virtual void OnActionStarted_Implementation(const FString& contextString = "") override;

    virtual void OnActionEnded_Implementation() override;

    virtual FName GetMontageSectionName_Implementation() override;

    virtual void OnSubActionStateEntered_Implementation() override;

    virtual void OnSubActionStateExited_Implementation() override;

    virtual FTransform GetWarpTransform_Implementation() override;

    UFUNCTION(BlueprintPure, Category = ACF)
    EACFDirection GetDodgeDirection() const
    {
        return finalDirection;
    }

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    TMap<EACFDirection, FName> DodgeDirectionToMontageSectionMap;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bShouldWarp), Category = "ACF| Warp")
    float DodgeLength = 600.f;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bShouldWarp), Category = "ACF| Warp")
    EACFDirection defaultDodgeDirection;

private:
    FVector dodgeDirection;

    EACFDirection finalDirection;
};
