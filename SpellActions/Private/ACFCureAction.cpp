// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFCureAction.h"
#include "Actions/ACFBaseAction.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFTypes.h"
#include "ARSStatisticsComponent.h"
#include "Actors/ACFCharacter.h"

void UACFCureAction::OnNotablePointReached_Implementation()
{
	Super::OnNotablePointReached_Implementation();

	AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);
	if (acfCharacter && acfCharacter->GetStatisticsComponent()) {
		acfCharacter->GetStatisticsComponent()->ModifyStat(StatModifier);
		UACFFunctionLibrary::PlayActionEffect(ActionConfig.ActionEffect, CharacterOwner, this);
	}
}
