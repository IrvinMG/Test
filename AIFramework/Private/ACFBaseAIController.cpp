// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFBaseAIController.h"
#include "Interfaces/ACFEntityInterface.h"

void AACFBaseAIController::OnPossess(APawn* possPawn)
{
	Super::OnPossess(possPawn);
	PossessedEntity = Cast<IACFEntityInterface>(GetPawn());
	SetGenericTeamId(FGenericTeamId(uint8(CombatTeam)));
}

void AACFBaseAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AACFBaseAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);

	CombatTeam = (ETeam)InTeamID.GetId();
	SetCombatTeam(CombatTeam);
}


FGenericTeamId AACFBaseAIController::GetGenericTeamId() const
{
	return uint8(CombatTeam);
}

void AACFBaseAIController::SetCombatTeam_Implementation(const ETeam& newTeam)
{
	const bool bImplements = GetPawn()->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
	if (bImplements)
	{
		IACFEntityInterface::Execute_AssignTeamToEntity(GetPawn(), CombatTeam);
	}
}

bool AACFBaseAIController::SetCombatTeam_Validate(const ETeam& newTeam)
{
	return true;
}