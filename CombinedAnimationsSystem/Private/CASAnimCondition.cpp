// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "CASAnimCondition.h"
#include "CASAnimSlaveComponent.h"
#include <GameFramework/Character.h>


bool UCASAnimCondition::Internal_VerifyCondition( class UCASAnimSlaveComponent* ownerComp, const FGameplayTag& animTag, const ACharacter* animMaster)
{
	ownerCompoment = ownerComp;
	return VerifyCondition(animTag, animMaster);

};

UWorld* UCASAnimCondition::GetWorld() const
{
	
	return ownerCompoment ? ownerCompoment->GetWorld() : nullptr;
}

bool UCASORCondition::VerifyCondition_Implementation(const FGameplayTag& animTag, const class ACharacter* animMaster)
{
	for (auto& cond : OrConditions)
	{
		if (cond->Internal_VerifyCondition(ownerCompoment, animTag, animMaster)) {
			return true;
		}
	}
	return false;
}
