// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Components/ACFAIPatrolComponent.h"
#include "ACFAIController.h"

// Sets default values for this component's properties
UACFAIPatrolComponent::UACFAIPatrolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UACFAIPatrolComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetPathToFollow()) {
		const APawn* pawn = Cast<APawn>(GetOwner());
		if (pawn) {
			AACFAIController* aiContr = Cast< AACFAIController>(pawn->GetController());
			if (aiContr) {
				aiContr->SetPatrolPath(GetPathToFollow());
			}
		}
	}
}


