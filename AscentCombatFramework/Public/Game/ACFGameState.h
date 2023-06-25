// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "ACFGameState.generated.h"


class UACFTeamManagerComponent;
class UACMEffectsDispatcherComponent;

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStateChanged, const EBattleState&, battleState);

UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFGameState : public AGameState {
    GENERATED_BODY()
private:
    void UpdateBattleState();

protected:
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    EBattleState gameState = EBattleState::EExploration;

    UPROPERTY()
    TArray<class AAIController*> InBattleAIs;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<UACMEffectsDispatcherComponent> EffectsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<UACFTeamManagerComponent> TeamManagerComponent;

public:
    AACFGameState();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EBattleState GetBattleState() const { return gameState; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACMEffectsDispatcherComponent* GetEffectsComponent() const { return EffectsComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFTeamManagerComponent* GetTeamManager() const { return TeamManagerComponent; }

    void AddAIToBattle(class AAIController* contr);
    void RemoveAIFromBattle(class AAIController* contr);

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnBattleStateChanged OnBattleStateChanged;
};
