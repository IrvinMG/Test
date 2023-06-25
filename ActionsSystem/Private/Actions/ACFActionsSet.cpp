// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Actions/ACFActionsSet.h"
#include "ACFActionTypes.h"
#include "GameplayTagContainer.h"




bool UACFActionsSet::GetActionByTag(const FGameplayTag& Action, FActionState& outAction) const
{
	const FActionState* actionState = Actions.FindByKey(Action);
	if (actionState) {
		outAction = *actionState;
		return true;
	}
	return false;
}

void UACFActionsSet::AddOrModifyAction(const FActionState& action)
{
	if (Actions.Contains(action.TagName)) {
		Actions.Remove(action);
	}
	Actions.AddUnique(action);
}


