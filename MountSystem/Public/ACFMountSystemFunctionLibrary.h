// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Game/ACFDamageType.h"
#include "ACFMountSystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTSYSTEM_API UACFMountSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFCharacter* GetLocalRiderPlayerCharacter(const UObject* WorldContextObject);


    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool DoesDamageInvolveLocalPlayer(const FACFDamageEvent& damageEvent, bool& bIsVictim);
	
};
