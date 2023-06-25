// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACFHUDUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class AACFHUD;

protected: 

	UFUNCTION(BlueprintImplementableEvent, Category = ACF)
	void OnPossessedCharacterChanged(const class AACFCharacter* newChar);

	UFUNCTION(BlueprintImplementableEvent, Category = ACF)
	void EnableCrosshair(bool bEnabled);
	
};
  