// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ACMTypes.h"
#include "ACMImpactsFXDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class COLLISIONSMANAGER_API UACMImpactsFXDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected: 

	UPROPERTY(EditDefaultsOnly, Category = ACM)
	TMap<TSubclassOf<class UDamageType>, FImpactsArray> ImpactFXsByDamageType;

public: 

	UFUNCTION(BlueprintCallable, Category = ACM)
	bool TryGetImpactFX(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, FBaseFX& outFXtoPlay);

};
