// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFGameState.h"
#include "ACMEffectsDispatcherComponent.h"
#include "AIController.h"
#include "Components/ACFTeamManagerComponent.h"

void AACFGameState::UpdateBattleState()
{
	const EBattleState state = InBattleAIs.Num() > 0 ? EBattleState::EBattle : EBattleState::EExploration;
	if (gameState != state)
	{
		gameState = state;
		OnBattleStateChanged.Broadcast(gameState);
	}
}

AACFGameState::AACFGameState()
{
	EffectsComp = CreateDefaultSubobject<UACMEffectsDispatcherComponent>(TEXT("Effects Component"));
	TeamManagerComponent = CreateDefaultSubobject<UACFTeamManagerComponent>(TEXT("Team Manager"));
}

void AACFGameState::AddAIToBattle(AAIController* contr)
{
	if (!contr) {
		return;
	}
		
	InBattleAIs.Add(contr);
	UpdateBattleState();
}

void AACFGameState::RemoveAIFromBattle(AAIController* contr)
{
	if (contr && InBattleAIs.Contains(contr))
	{
		InBattleAIs.Remove(contr);
		UpdateBattleState();
	}
}
