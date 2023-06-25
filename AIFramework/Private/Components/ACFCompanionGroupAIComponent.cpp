// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFCompanionGroupAIComponent.h"
#include "Actors/ACFCharacter.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFPlayerController.h"
#include "ARSStatisticsComponent.h"
#include <GameFramework/Controller.h>

void UACFCompanionGroupAIComponent::AddExpToTheGroup(int32 Exp) {

  if (contr) {
    AACFCharacter *charGroupLead = Cast<AACFCharacter>(contr->GetPawn());
    if (charGroupLead) {
      UARSStatisticsComponent *StatComp =  charGroupLead->GetStatisticsComponent();
      if (StatComp) {
		  StatComp->AddExp(Exp);
      }
    }
	for (auto& companion : AICharactersInfo) {
		if (companion.AICharacter)	{
			UARSStatisticsComponent* StatComp = companion.AICharacter->GetStatisticsComponent();
			if (StatComp) {
				StatComp->AddExp(Exp);
			}
		}
	}
  }
}

void UACFCompanionGroupAIComponent::SetReferences() {
  /*Super::SetReferences();*/
  contr = Cast<AController>(GetOwner());
  if (contr) {
    AACFCharacter *charGroupLead = Cast<AACFCharacter>(contr->GetPawn());
    if (charGroupLead) {
      charGroupLead->OnDamageInflicted.AddDynamic(this, &UACFCompanionGroupAIComponent::HandleLeadHits);
      charGroupLead->OnDamageReceived.AddDynamic(this, &UACFCompanionGroupAIComponent::HandleLeadGetHit); 
    }
	groupLead = contr->GetPawn(); 
  }
}

void UACFCompanionGroupAIComponent::BeginPlay()
{
	Super::BeginPlay();
	contr = Cast<AController>(GetOwner());
	if (contr) {
		auto& eventNewPawn = contr->GetOnNewPawnNotifier();
		eventNewPawn.AddLambda([this](APawn* newPawn) {
			HandlePossessedCharacterChanged(Cast<AACFCharacter>(newPawn));
			}
		);
	}
}

void UACFCompanionGroupAIComponent::HandleLeadGetHit(  const FACFDamageEvent &damage) {
	 SetInBattle(true, damage.DamageDealer);
}

void UACFCompanionGroupAIComponent::HandleLeadHits( class AActor *damaged) {
	  SetInBattle(true, damaged);
}

void UACFCompanionGroupAIComponent::HandlePossessedCharacterChanged(const AACFCharacter *_char) {
  if (contr && _char) {
    AACFCharacter *charLead = Cast<AACFCharacter>(groupLead);
    if (charLead) {
      charLead->OnDamageInflicted.RemoveDynamic(
          this, &UACFCompanionGroupAIComponent::HandleLeadHits);
      charLead->OnDamageReceived.RemoveDynamic(
          this, &UACFCompanionGroupAIComponent::HandleLeadGetHit);
      SetReferences();
    }
  }
}
