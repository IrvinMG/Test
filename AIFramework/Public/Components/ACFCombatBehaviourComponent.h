// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFTypes.h"
#include "ACFAITypes.h"
#include <Components/ActorComponent.h>
#include "ACFCombatBehaviourComponent.generated.h"

struct FACFDamageEvent;

/**
 * 
 */

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class AIFRAMEWORK_API UACFCombatBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

	UACFCombatBehaviourComponent();
public: 

	friend class AACFAIController;

	/*The default combat behavior (melee/ranged) for this AI. Could change during combat if multiple 
	behaviors have been defined in Allowed Behaviors*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
	ECombatBehaviorType DefaultCombatBehaviorType;

	/*The default combat state  for this AI, triggered once no other states are available*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
	EAICombatState DefaultCombatState = EAICombatState::EMeleeCombat;

// 	/*The minimum amount of Actions that should be done in MeleeAttacks state before going in
// 	Post Attack*/
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
// 	uint8 MinMeleeComboAttacks = 1;
// 
// 	/*The maximum amount of Actions that should be done in MeleeAttacks state before going in
// 	Post Attack*/
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
// 	uint8 MaxMeleeComboAttacks = 2;

	/* If this ai needs an equipped weapon to start fighting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
	bool bNeedsWeapon = false;

	/* Action to be triggered by this ai to equip a Melee weapon*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "ACF")
	FGameplayTag EquipMeleeAction;

	/* Action to be triggered by this ai to equip a Ranged weapon*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "ACF")
	FGameplayTag EquipRangedAction;

	/* Action to be triggered when combat starts*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "ACF")
	FGameplayTag EngagingAction;

	/* The allowed combat behaviors (melee / ranged) of this AI*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
	TArray<ECombatBehaviorType> AllowedBehaviors;

	/*Configuration of each Combat State*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF")
	TArray<FAICombatStateConfig> CombatStatesConfig;

	/*The actions that should be performed by the AI for every combat state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Actions")
	TMap<EAICombatState, FActionsChances> ActionByCombatState;


	/*Generic conditionals action you can define by creating your own ActionCondition class*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Actions")
	TArray<FConditions> ActionByCondition;

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryExecuteActionByCombatState(EAICombatState combatState);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryExecuteConditionAction();

	

	UFUNCTION(BlueprintPure, Category = ACF)
	bool IsTargetInMeleeRange(AActor* target);

	UFUNCTION(BlueprintPure, Category = ACF)
	EAICombatState GetBestCombatStateByTargetDistance(float targetDistance);

	UFUNCTION(BlueprintCallable, Category = ACF)
	float GetIdealDistanceByCombatState(EAICombatState combatState) const;

	bool EvaluateCombatState(EAICombatState combatState);


private: 

	bool VerifyCondition(const FConditions& condition);

	void InitBehavior(class AACFAIController* _controller);

	void TryEquipWeapon();

	void UninitBehavior();

	bool CheckEquipment();

	

	UPROPERTY()
	class AACFCharacter* characterOwner;

	UPROPERTY()
	class AACFAIController* aiController;
	 
/*	void UpdateBehaviorType();*/
};
