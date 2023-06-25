// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ARSTypes.h"
#include "ACFBuffAction.generated.h"

/**
 * 
 */
UCLASS()
class SPELLACTIONS_API UACFBuffAction : public UACFBaseAction
{
	GENERATED_BODY()
	
protected: 

    virtual void OnNotablePointReached_Implementation() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF )
	FTimedAttributeSetModifier BuffToApply;
};
