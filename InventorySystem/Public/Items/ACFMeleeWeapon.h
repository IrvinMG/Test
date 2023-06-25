// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFWeapon.h"
#include <GameplayTagContainer.h>
#include "ACFMeleeWeapon.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, FHitResult, HitResult);



UCLASS()
class INVENTORYSYSTEM_API AACFMeleeWeapon : public AACFWeapon
{
	GENERATED_BODY()

public:

	AACFMeleeWeapon();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnWeaponHit OnWeaponHit;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void StartWeaponSwing(const TArray<FName>& traceChannels);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void StopWeaponSwing();


protected:

	virtual void OnWeaponUnsheathed_Implementation() override;

	virtual void OnWeaponSheathed_Implementation() override;

private:

	UFUNCTION()
	void HandleAttackHit(const FHitResult& HitResult);

	UPROPERTY()
	float VFXCylinderHeight;

};
