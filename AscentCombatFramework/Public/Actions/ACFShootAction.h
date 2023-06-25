// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFItemTypes.h"
#include "ACFShootAction.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFShootAction : public UACFBaseAction
{
	GENERATED_BODY()

	UACFShootAction();

	virtual bool CanExecuteAction_Implementation(class ACharacter* owner) override;
protected:

	virtual void OnNotablePointReached_Implementation() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ACF)
	TSubclassOf<class AACFProjectile> ProjectileOverride;

	/*The direction offset from the controller position*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ACF)
	FRotator DeltaDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ACF)
	EShootTargetType SwipeTargetType;
};
