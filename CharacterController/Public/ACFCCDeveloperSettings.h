// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Engine/DeveloperSettings.h"
#include "ACFCCDeveloperSettings.generated.h"


UCLASS(config = Plugins, Defaultconfig, meta = (DisplayName = "Ascent Character Controller Settings"))
class CHARACTERCONTROLLER_API UACFCCDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UACFCCDeveloperSettings();

	UPROPERTY(EditAnywhere, config, Category = "ACF | Root Tags")
	FGameplayTag MovesetTypeTag;

	UPROPERTY(EditAnywhere, config, Category = "ACF | Root Tags")
	FGameplayTag AnimationOverlayTag;

};
   