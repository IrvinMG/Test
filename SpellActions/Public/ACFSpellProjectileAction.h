// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFSpellProjectileAction.generated.h"

/**
 * 
 */
UCLASS()
class SPELLACTIONS_API UACFSpellProjectileAction : public UACFBaseAction
{
	GENERATED_BODY()


protected: 

	virtual void OnNotablePointReached_Implementation() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
	TSubclassOf<class AACFProjectile> ProjectileClass;
	
};
