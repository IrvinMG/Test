   // Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFPlayerController.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFInteractionComponent.h"
#include "ATSTargetingComponent.h"
#include "CCMPlayerCameraManager.h"
#include <TimerManager.h>
#include <Components/ActorComponent.h>
#include <Net/UnrealNetwork.h>
#include "Interfaces/ACFEntityInterface.h"
#include "GameFramework/PlayerController.h"





AACFPlayerController::AACFPlayerController()
{
}

void AACFPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AACFPlayerController, PossessedCharacter);
}

void AACFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	secondsFromLastCameraInput = 0.f;
}


void AACFPlayerController::Tick(float DeltaSeconds)
{
    if (FMath::IsNearlyZero(RotationInput.Yaw)) {
		const FTimespan timeDiff = FDateTime::Now() - lastInput;
		secondsFromLastCameraInput = timeDiff.GetTotalSeconds();
	}
	else {
		lastInput = FDateTime::Now();
		secondsFromLastCameraInput = 0.f;
	}
	Super::Tick(DeltaSeconds);
}


float AACFPlayerController::GetXSensitivity_Implementation() const
{
	return 1.f;
}

float AACFPlayerController::GetYSensitivity_Implementation() const
{
	return 1.f;
}

void AACFPlayerController::SetCombatTeam_Implementation(const ETeam& newTeam)
{
	const bool bImplements = GetPawn()->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
	if (bImplements)
	{
		IACFEntityInterface::Execute_AssignTeamToEntity(GetPawn(), newTeam);		
	}
}

bool AACFPlayerController::SetCombatTeam_Validate(const ETeam& newTeam)
{
	return true;
}



void AACFPlayerController::OnRep_PossessedEntity()
{
	HandleNewEntityPossessed();
}

void AACFPlayerController::HandleNewEntityPossessed()
{
	PossessedEntity = Cast<IACFEntityInterface>(GetPawn());
	PossessedCharacter = Cast<AACFCharacter>(GetPawn());
	OnPossessedCharacterChanged.Broadcast(PossessedCharacter);
}

void AACFPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	SetCombatTeam(CombatTeam);
	EnableCharacterComponents(true);
	HandleNewEntityPossessed();	   
}

void AACFPlayerController::EnableCharacterComponents(bool bEnabled)
{
	UACFInteractionComponent* interComponent = GetPawn()->FindComponentByClass<UACFInteractionComponent>();

	if (interComponent)
	{
		if (bEnabled)
			interComponent->RegisterComponent();
		else
			interComponent->UnregisterComponent();
	}
}

void AACFPlayerController::OnUnPossess()
{
	EnableCharacterComponents(false);
	Super::OnUnPossess();
}

