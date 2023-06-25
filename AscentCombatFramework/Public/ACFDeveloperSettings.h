// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Curves/CurveFloat.h>
#include "Game/ACFTypes.h"
#include "Engine/DeveloperSettings.h"
#include "ACFDeveloperSettings.generated.h"

enum class EDamageZone : uint8;

UCLASS(config = Plugins, Defaultconfig, meta = (DisplayName = "Ascent Combat Settings"))
class ASCENTCOMBATFRAMEWORK_API UACFDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UACFDeveloperSettings();

	UPROPERTY(EditAnywhere, config, Category = "ACF | Default Tags")
	FGameplayTag DefaultHitState;

	UPROPERTY(EditAnywhere, config, Category = "ACF | Default Tags")
	FGameplayTag DefaultDeathState;

	UPROPERTY(EditAnywhere, config, Category = "ACF | Default Tags")
	FGameplayTag HealthTag;

// 	UPROPERTY(EditAnywhere, config, Category = "ACF | Default Classes")
// 	TSubclassOf<class UACFDamageCalculation> DamageCalculatorClass;
	
};
   