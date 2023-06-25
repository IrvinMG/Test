// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFExecutionCondition.h"
#include "ARSStatisticsComponent.h"
#include "CASAnimCondition.h"
#include "CASAnimSlaveComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include <GameFramework/Character.h>
#include "GameplayTagsManager.h"

bool UACFExecutionCondition::VerifyCondition_Implementation(const FGameplayTag& animTag, const ACharacter* animMaster)
{
	if (FMath::RandRange(0.f, 100.f) > ExecutionChance) {
		return false;
	}
	if (!animMaster) {
		return false;
	}

	const UARSStatisticsComponent* ownerStatComp = GetOwnerComponent()->GetCharacterOwner()->FindComponentByClass< UARSStatisticsComponent>();
	if (ownerStatComp) {

		const FGameplayTag healthTag = UACFFunctionLibrary::GetHealthTag();
		const float health = ownerStatComp->GetNormalizedValueForStatitstic(healthTag);

		if ((RemainingHealthPercentage / 100.f) > health) {
			return true;
		}
	}

	return false;

}

bool UACFIsChildTagCondition::VerifyCondition_Implementation(const FGameplayTag& animTag, const class ACharacter* animMaster)
{
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(AnimRootTag).HasTag(animTag);
}
