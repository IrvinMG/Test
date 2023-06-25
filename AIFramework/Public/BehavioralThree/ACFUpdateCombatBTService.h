// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"
#include "Game/ACFTypes.h"

#include "ACFUpdateCombatBTService.generated.h"


/**
 *
 */
UCLASS()
class AIFRAMEWORK_API UACFUpdateCombatBTService : public UBTService {
    GENERATED_BODY()

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    void SetNewMaxComboCount();

    void UpdateCombat();

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr < class AACFAIController> aiController;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr < class AActor> targetActor;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr<class AACFCharacter> CharOwner;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFCombatBehaviourComponent> combatBehav;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    FGameplayTag SwitchCombatTypeActionTag;
};
