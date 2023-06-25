// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL
// 2022. All Rights Reserved.

#pragma once

#include "ARSTypes.h"
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ARSGenerationRulesDataAsset.h"
#include <GameplayTagContainer.h>

#include "ARSDeveloperSettings.generated.h"

/**
 *
 */
class UARSGenerationRulesDataAsset;

UCLASS(config = Plugins, defaultconfig, meta = (DisplayName = "Ascent RPG System"))
class ADVANCEDRPGSYSTEM_API UARSDeveloperSettings : public UDeveloperSettings {
    GENERATED_BODY()

public:
    /*Define here the tag Root of your Primary Attributes*/
    UPROPERTY(EditAnywhere, config, Category = ARS)
    FGameplayTag PrimaryAttributesRootTag;

    /*Define here the tag Root of your Statistics*/
    UPROPERTY(EditAnywhere, config, Category = ARS)
    FGameplayTag StatisticsRootTag;

    /*Define here the tag Root of your Attributes*/
    UPROPERTY(EditAnywhere, config, Category = ARS)
    FGameplayTag AttributesRootTag;

    /*Max Level for all your character*/
    UPROPERTY(EditAnywhere, config, Category = ARS)
    int32 MaxLevel = 100;

    UARSGenerationRulesDataAsset* GetAttributesGenerationRules() const
    {
        return Cast<UARSGenerationRulesDataAsset>(AttributesGenerationConfig.TryLoad());
    }

protected:
    /*Define here the tag Root of your Primary Attributes*/  
	UPROPERTY(config, EditAnywhere, Category = ARS, meta = (AllowedClasses = "ARSGenerationRulesDataAsset"))
    FSoftObjectPath AttributesGenerationConfig;
};
