// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "ACFActionsDeveloperSettings.generated.h"

/**
 *
 */
UCLASS(config = Plugins, Defaultconfig, meta = (DisplayName = "Ascent Actions System Settings"))
class ACTIONSSYSTEM_API UACFActionsDeveloperSettings : public UDeveloperSettings {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, config, Category = "ACF | Root Tags")
    FGameplayTag MovesetActionsRootTag;

    UPROPERTY(EditAnywhere, config, Category = "ACF | Default Tags")
    FGameplayTag DefaultActionsState;
};
