// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ACMCollisionsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COLLISIONSMANAGER_API UACMCollisionsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
		
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACM)
	static UACMEffectsDispatcherComponent* GetEffectDispatcher(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACM)
	static void PlayImpactEffect(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, const FVector& impactLocation, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACM)
	static void PlayReplicatedEffect(const FImpactFX& FXtoPlay, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACM)
    static void PlayEffectLocally(const FImpactFX& FXtoPlay, const UObject* WorldContextObject);
	
};
