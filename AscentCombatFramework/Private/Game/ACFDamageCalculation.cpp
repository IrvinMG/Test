// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFDamageCalculation.h"





float UACFDamageCalculation::CalculateFinalDamage_Implementation(const FACFDamageEvent& inDamageEvent)
{
    return inDamageEvent.FinalDamage;
}

FGameplayTag UACFDamageCalculation::EvaluateHitResponseAction_Implementation(const FACFDamageEvent& inDamageEvent, 
	const TArray<FOnHitActionChances>& hitResponseActions)
{
    return FGameplayTag();
}

bool UACFDamageCalculation::IsCriticalDamage_Implementation(const FACFDamageEvent& inDamageEvent)
{
	return false;
}

