// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/ACFDamageType.h"
#include "ACFDamageWidget.generated.h"

struct FACFDamageEvent;

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public: 

 	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = ACF)
 	void SetupDamageWidget(const FACFDamageEvent& damageEvent);

	
};
