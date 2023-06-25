// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ARSTypes.h"
#include "Actions/ACFBaseAction.h"
#include "CoreMinimal.h"

#include "ACFCureAction.generated.h"

/**
 *
 */
UCLASS()
class SPELLACTIONS_API UACFCureAction : public UACFBaseAction {
    GENERATED_BODY()

  protected:
    virtual void OnNotablePointReached_Implementation() override;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF)
    FStatisticValue StatModifier;
};
