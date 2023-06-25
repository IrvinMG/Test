// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved.

#pragma once

#include "ACFDeveloperSettings.h"
#include "ACFTypes.h"
#include "CoreMinimal.h"
#include "Game/ACFDamageCalculation.h"
#include "GameFramework/DamageType.h"
#include "GameplayTagContainer.h"
#include "ACFDamageType.h"

#include "ACFDamageTypeCalculator.generated.h"

class UDamageType;
struct FDamageInfluence;
struct FOnHitActionChances;

/**
 *
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFDamageTypeCalculator : public UACFDamageCalculation {
    GENERATED_BODY()

    UACFDamageTypeCalculator();

protected:
    /*For every hitresponse you can define a multiplier to be applied to the final damage*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF|Hit Responses")
    TMap<FGameplayTag, float> HitResponseActionMultiplier;

    /*For every damagetype, the parameter to be used to calculate crit chance. 100 means always crit*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF| Critical Damage Config")
    TMap<TSubclassOf<UDamageType>, FDamageInfluence> CritChancePercentageByParameter;

    /*Multiplier applied when an hit is Critical*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF| Critical Damage Config")
    float critMultiplier = 1.5f;

    /*This statistic is used to prevent the owner to go on Hit Action ( = being staggered).
    If set, this statistic will be reduced by the actual damage at every hit and the owner
    will only be staggered when this statistic reaches 0. (make sure regen is activate, otherwise
    the AI after the first stagger, will be always be staggered) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF|Hit Responses")
    FGameplayTag StaggerResistanceStastistic;

    /*For every damagetype, the parameters that will be used to calculate the damage and a scaling factor
    Total Damage will be the sum of all ATTACK DAMAGE INFLUENCES of the DAMAGE DEALER  * scalingfactor */
    /*DEFENSE PARAMETER PERCENTAGES are the parameters that will be used from the DAMAGE RECEIVER to
    REDUCE the incoming damage and a scaling factor.
    Total Damage will be reduced by the sum of all the influences * scalingfactor,  in percentages.
    if the sum of all those parameters scaled is 30, incoming damage will be reduced by 30%.
    If this number is >= 100, damage will be reduced to 0*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
    TMap<TSubclassOf<UDamageType>, FDamageInfluences> DamageInfluencesByParameter;

    /*Random deviation in percentage added to final damage. % means that final damage will be randomized
    in the range -5% , +5%*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
    float DefaultRandomDamageDeviationPercentage = 5.0f;

    /*Multiplier applied depending on the damaged zone of the receiver*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
    TMap<EDamageZone, float> DamageZoneToDamageMultiplier;

    /*The parameter to be used to reduce incoming damage when in Defense State*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
    FGameplayTag DefenseStanceParameterWhenBlocked;

    virtual float CalculateFinalDamage_Implementation(const FACFDamageEvent& inDamageEvent) override;

    virtual FGameplayTag EvaluateHitResponseAction_Implementation(const FACFDamageEvent& inDamageEvent, 
        const TArray<FOnHitActionChances>& hitResponseActions) override;

    virtual bool IsCriticalDamage_Implementation(const FACFDamageEvent& inDamageEvent) override;

private:
  
    bool EvaluetHitResponseAction(const FOnHitActionChances& action, const FACFDamageEvent& damageEvent);
};
