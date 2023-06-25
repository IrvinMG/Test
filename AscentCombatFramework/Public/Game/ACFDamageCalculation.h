// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFDamageType.h"
#include "CoreMinimal.h"

#include "ACFDamageCalculation.generated.h"

struct FACFDamageEvent;

/**
 *
 */
UCLASS(Blueprintable, Abstract, BlueprintType, DefaultToInstanced, EditInlineNew)
class ASCENTCOMBATFRAMEWORK_API UACFDamageCalculation : public UObject {
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    float CalculateFinalDamage(const FACFDamageEvent& inDamageEvent);
    virtual float CalculateFinalDamage_Implementation(const FACFDamageEvent& inDamageEvent);

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    FGameplayTag EvaluateHitResponseAction(const FACFDamageEvent& inDamageEvent, const TArray<FOnHitActionChances>& hitResponseActions);
    virtual FGameplayTag EvaluateHitResponseAction_Implementation(const FACFDamageEvent& inDamageEvent, const TArray<FOnHitActionChances>& hitResponseActions);

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    bool IsCriticalDamage(const FACFDamageEvent& inDamageEvent);
    virtual bool IsCriticalDamage_Implementation(const FACFDamageEvent& inDamageEvent);
};
