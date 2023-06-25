// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "Game/ACFDamageType.h"
#include "ACFHitAction.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFHitAction : public UACFBaseAction
{
	GENERATED_BODY()

		UACFHitAction();

protected: 

	virtual void OnActionStarted_Implementation(const FString& contextString = "") override;
	virtual void OnActionEnded_Implementation() override;

	virtual FName GetMontageSectionName_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TMap<EACFDirection, FName> HitDirectionToMontageSectionMap;

	FACFDamageEvent damageReceived;
};
