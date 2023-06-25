// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "ARSTypes.h"
#include "GameFramework/DamageType.h"
#include "Game/ACFTypes.h"

#include "ACFBlockComponent.generated.h"


UCLASS(ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFBlockComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFBlockComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    /*Modifier applied while defending*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FAttributesSetModifier DefendingModifier;

    /*Directions in which incoming hits are blocked*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    EActionDirection BlockDirection;

    /*Types of damages that can be blocked*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    TArray<TSubclassOf<UDamageType>> BlockableDamages;

    /*Multiplier for the damaged applied to the DamagedStatistic at every damage*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    float DamagedStatisticMultiplier = 1.f;

    /*Types of damages that can be countered*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    TArray<TSubclassOf<UDamageType>> CounterableDamages;



public:
    UFUNCTION(BlueprintPure, Category = ACF)
    FAttributesSetModifier GetDefendingModifier() const { return DefendingModifier; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetDefendingModifier(const FAttributesSetModifier& val) { DefendingModifier = val; }

    UFUNCTION(BlueprintPure, Category = ACF)
    EActionDirection GetBlockDirection() const { return BlockDirection; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetBlockDirection(EActionDirection val) { BlockDirection = val; }

    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<TSubclassOf<UDamageType>> GetBlockableDamages() const { return BlockableDamages; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetBlockableDamages(TArray<TSubclassOf<UDamageType>> val) { BlockableDamages = val; }

    UFUNCTION(BlueprintPure, Category = ACF)
    float GetDamagedStatisticMultiplier() const { return DamagedStatisticMultiplier; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetDamagedStatisticMultiplier(float val) { DamagedStatisticMultiplier = val; }

    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<TSubclassOf<UDamageType>> GetCounterableDamages() const { return CounterableDamages; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetCounterableDamages(TArray<TSubclassOf<UDamageType>> val) { CounterableDamages = val; }

};
