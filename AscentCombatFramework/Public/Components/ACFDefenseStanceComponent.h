// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ARSTypes.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFTypes.h"

#include "ACFDefenseStanceComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefenseStanceChanged, bool, bIsDefending);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCounterAttackTriggered);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFDefenseStanceComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFDefenseStanceComponent();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void StartDefending();

    UFUNCTION(Server,  Reliable, BlueprintCallable, Category = ACF)
    void StopDefending();

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool TryBlockIncomingDamage(const FACFDamageEvent& damageEvent, float damageToBlock, FGameplayTag& outAction);

    UFUNCTION(BlueprintPure, Category = ACF)
    bool CanBlockDamage(const FACFDamageEvent& damageEvent) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool CanStartDefensePosition() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsInDefensePosition() const
    {
        return bIsInDefensePosition;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetBlockAction() const
    {
        return ActionToBeTriggeredOnBlock;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetDamagedStatistic() const
    {
        return DamagedStatistic;
    }

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnDefenseStanceChanged OnDefenseStanceChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnCounterAttackTriggered OnCounterAttackTriggered;

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFBlockComponent* GetCurrentBlockComp() const { return currentBlockComp; }

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    /*To start go in DefenseMode, minimum value of the DamagedStatistic to start defense mode*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    float MinimumDamageStatisticToStartBlocking = 5.f;

    /*Action triggered on the owner of this component when an hit is successfully blocked*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FGameplayTag ActionToBeTriggeredOnBlock;

    /*Action triggered on the owner of this component when the defence is broken*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FGameplayTag ActionToBeTriggeredOnDefenceBreak;

    /*Damage received during Defense will be  removed from this statistic.
    When this statistic reaches 0, defense will be broken
    Usually, damaged statistic is Stamina*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FGameplayTag DamagedStatistic;

    /*Action triggered on the owner of this component when an hit is successfully countered*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FGameplayTag CounterAttackAction;


    /*If the owner can block while unarmed*/
    //     UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    //     bool CanBlockWhenUnarmed = false;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void EnableCounterAttackWindow()
    {
        bCounterGate = true;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void DisableCounterAttackWindow()
    {
        bCounterGate = false;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool CanCounterAttack(const FACFDamageEvent& incomingDamage);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool TryCounterAttack(const FACFDamageEvent& incomingDamage);

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFBlockComponent* TryGetBestBlockComp() const;

    UFUNCTION()
    void OnRep_DefPos();

private:
    UPROPERTY(ReplicatedUsing = OnRep_DefPos)
    bool bIsInDefensePosition = false;

    bool bCounterGate = false;

    TObjectPtr<class UARSStatisticsComponent> statComp;
    TObjectPtr<class UACFCharacterMovementComponent> locComp;
    class UACFBlockComponent* currentBlockComp;

    UFUNCTION()
    void HandleEquipmentChanged(const FEquipment& Equipment);
};
