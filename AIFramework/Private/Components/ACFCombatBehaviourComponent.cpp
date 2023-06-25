// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFCombatBehaviourComponent.h"
#include "Actors/ACFCharacter.h"
#include "ACFAIController.h"
#include "Components/ACFEquipmentComponent.h"
#include "Game/ACFTypes.h"
#include "ACFActionCondition.h"
#include "Game/ACFFunctionLibrary.h"

UACFCombatBehaviourComponent::UACFCombatBehaviourComponent()
{
	FAICombatStateConfig melee = FAICombatStateConfig(EAICombatState::EMeleeCombat, 100.f, ELocomotionState::EWalk);
	UACFDistanceActionCondition* distanceCond = melee.GetDistanceBasedCondition();
	if (distanceCond) {
		distanceCond->SetContidionConfig(EConditionType::EBelow, 120.f);
	}
	CombatStatesConfig.Add(melee);


	FAICombatStateConfig studyTarget = FAICombatStateConfig(EAICombatState::EStudyTarget, 50.f, ELocomotionState::EWalk);
	UACFDistanceActionCondition* studyDistanceCond = studyTarget.GetDistanceBasedCondition();
	if (studyDistanceCond) {
		studyDistanceCond->SetContidionConfig(EConditionType::EEqual, 300.f, 100.f);
	}
	CombatStatesConfig.Add(studyTarget);

	FAICombatStateConfig chase = FAICombatStateConfig(EAICombatState::EChaseTarget, 100.f, ELocomotionState::EJog);
	UACFDistanceActionCondition* chaseCond = chase.GetDistanceBasedCondition();
	if (chaseCond) {
		chaseCond->SetContidionConfig(EConditionType::EBelow, 800.f);
	}
	CombatStatesConfig.Add(chase);


	FAICombatStateConfig ranged = FAICombatStateConfig(EAICombatState::ERangedCombat, 100.f, ELocomotionState::EWalk);
	UACFDistanceActionCondition* rangedCond = ranged.GetDistanceBasedCondition();
	if (rangedCond) {
		rangedCond->SetContidionConfig(EConditionType::EAbove, 500.f);
	}
	CombatStatesConfig.Add(ranged);
	AllowedBehaviors.Add(ECombatBehaviorType::EMelee);
}



bool UACFCombatBehaviourComponent::TryExecuteActionByCombatState(EAICombatState combatState)
{
	if (CheckEquipment())
	{
		TryEquipWeapon();
		return false;
	}

	FActionsChances* actions = ActionByCombatState.Find(combatState);
	if (actions)
	{
		for (auto& elem : actions->PossibleActions)
		{
			if (UACFFunctionLibrary::ShouldExecuteAction(elem, characterOwner))
			{
				aiController->SetWaitDurationTimeBK(elem.BTWaitTime);
				characterOwner->TriggerAction(elem.ActionTag, elem.Priority);
				return true;
			}
		}
	}
	return false;
}

bool UACFCombatBehaviourComponent::TryExecuteConditionAction()
{
	if (CheckEquipment())
	{
		TryEquipWeapon();
		return false;
	}

	for (auto actionCond : ActionByCondition)
	{
		FActionChances * action = &actionCond;
		if (VerifyCondition(actionCond) && (UACFFunctionLibrary::ShouldExecuteAction(*action, characterOwner)))
		{
			aiController->SetWaitDurationTimeBK(actionCond.BTWaitTime);
			characterOwner->TriggerAction(actionCond.ActionTag, actionCond.Priority);
			return true;
		}
	}
	return false;
}


bool UACFCombatBehaviourComponent::VerifyCondition(const FConditions& condition)
{
	return condition.Condition && condition.Condition->IsConditionMet(characterOwner);
}





bool UACFCombatBehaviourComponent::IsTargetInMeleeRange( AActor* target )
{
	const FAICombatStateConfig *meleeDist = CombatStatesConfig.FindByKey(EAICombatState::EMeleeCombat);

	const ACharacter* targetChar = Cast<ACharacter>(target);
	const float meleeDistance = GetIdealDistanceByCombatState(EAICombatState::EMeleeCombat);
	if (meleeDist) {
		if (targetChar) {
			const float dist = UACFFunctionLibrary::CalculateDistanceBetweenCharactersExtents(characterOwner, targetChar);
			return meleeDistance >= dist;
		}
		else if(target) {
			return characterOwner->GetDistanceTo(target) <= meleeDistance;
		}
	}		
	return false;
}



EAICombatState UACFCombatBehaviourComponent::GetBestCombatStateByTargetDistance(float targetDistance)
{
	if (targetDistance == 0) {
		characterOwner->TriggerAction(EngagingAction, EActionPriority::EMedium);
	//	return EAICombatState::EFlee;

	}

	for (const FAICombatStateConfig& state : CombatStatesConfig) {
		if (EvaluateCombatState(state.CombatState)) {
			return state.CombatState;
		}
	}


	return DefaultCombatState;
}

float UACFCombatBehaviourComponent::GetIdealDistanceByCombatState(EAICombatState combatState) const
{
	const FAICombatStateConfig* aiState = CombatStatesConfig.FindByKey(combatState);
	if (aiState) {
		const UACFDistanceActionCondition* distanceCond = aiState->GetDistanceBasedCondition();
		if (distanceCond) {
			return distanceCond->GetDistance();
		}	
	}

	UE_LOG(LogTemp, Warning, TEXT("Unallowed Combat State! - UACFCombatBehaviourComponent::GetIdealDistanceByCombatState"));
	return -1.f;
}



void UACFCombatBehaviourComponent::InitBehavior(class AACFAIController* _controller)
{
	if (_controller)
	{
		aiController = _controller;

		characterOwner = Cast<AACFCharacter>(_controller->GetPawn());

		if (characterOwner && characterOwner->GetACFCharacterMovementComponent()) {
			characterOwner->GetACFCharacterMovementComponent()->ResetStrafeMovement();
		}

		if (CheckEquipment())
		{
			TryEquipWeapon();
		}
	}
}

void UACFCombatBehaviourComponent::TryEquipWeapon()
{
	UACFEquipmentComponent* equipComp = characterOwner->GetEquipmentComponent();

	ensure(equipComp);

	if (DefaultCombatBehaviorType == ECombatBehaviorType::EMelee)
	{
		characterOwner->TriggerAction(EquipMeleeAction, EActionPriority::EHigh);
		aiController->SetCombatStateBK(EAICombatState::EMeleeCombat);
	}
	else if (DefaultCombatBehaviorType == ECombatBehaviorType::ERanged)
	{
		characterOwner->TriggerAction(EquipRangedAction, EActionPriority::EHigh);
		aiController->SetCombatStateBK(EAICombatState::ERangedCombat);
	}

}

void UACFCombatBehaviourComponent::UninitBehavior()
{

	if (bNeedsWeapon && characterOwner && characterOwner->GetCombatType() != ECombatType::EUnarmed)
	{
		const FGameplayTag unequipAction = DefaultCombatBehaviorType == ECombatBehaviorType::EMelee ? EquipMeleeAction : EquipRangedAction;
		characterOwner->TriggerAction(unequipAction, EActionPriority::EHighest);
	}
}

bool UACFCombatBehaviourComponent::CheckEquipment()
{	
	return (characterOwner && characterOwner->GetCombatType() != ECombatType::EMelee && bNeedsWeapon && DefaultCombatBehaviorType == ECombatBehaviorType::EMelee) ||
		(characterOwner && characterOwner->GetCombatType() != ECombatType::ERanged && bNeedsWeapon && DefaultCombatBehaviorType == ECombatBehaviorType::ERanged) ;	   
}


bool UACFCombatBehaviourComponent::EvaluateCombatState(EAICombatState combatState)
{
	if (!CombatStatesConfig.Contains(combatState)) {
		return false;
	}
	const FAICombatStateConfig* chance = CombatStatesConfig.FindByKey(combatState);

	if (chance) {
		for (auto condition : chance->Conditions) {
			if (!condition) {
				UE_LOG(LogTemp, Error, TEXT("INVALID ACTION CONDITION IN COMBAT CONFIG! - UACFCombatBehaviourComponent"));
				continue;
			}
			if (condition && !condition->IsConditionMet(characterOwner)) {
				return false;
			}
		}
		return FMath::RandRange(0.f, 100.f) <= chance->TriggerChancePercentage;
	}
	return false;
}



// void UACFCombatBehaviourComponent::UpdateBehaviorType()
// {
// 	if (IdealDistanceByCombatState.Contains(ECombatBehaviorType::ERanged))
// 	{	
// 		float* randegdist = IdealDistanceByCombatState.Find(ECombatBehaviorType::ERanged);
// 		if (randegdist &&  aiController->GetTargetActorDistanceBK() > *randegdist)
// 		{
// 			CurrentCombatBehaviorType = ECombatBehaviorType::ERanged;
// 			return;
// 		}			
// 	}
// 	CurrentCombatBehaviorType = ECombatBehaviorType::EMelee;
// }
