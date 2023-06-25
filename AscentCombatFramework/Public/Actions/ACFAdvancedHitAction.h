// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "Game/ACFDamageType.h"
#include "ACFAdvancedHitAction.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFAdvancedHitAction : public UACFBaseAction
{
	GENERATED_BODY()

	UACFAdvancedHitAction();

protected: 

	virtual void OnActionStarted_Implementation(const FString& contextString = "") override;
	virtual void OnActionEnded_Implementation() override;

	virtual FTransform GetWarpTransform_Implementation() override;
	virtual FName GetMontageSectionName_Implementation() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ACF)
	TMap<EHitDirection, FName> HitDirectionToMontageSectionMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ACF)
	TArray<FBoneSections> FrontDetailsSectionByBoneNames;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bSnapToTarget), Category = "ACF| Warp")
	float hitWarpDistance = 200.f;


private: 

	FName GetMontageSectionFromHitDirectionSafe(const EHitDirection hitDir) const;

	FName GetMontageSectionFromFront(const FACFDamageEvent& damageEvent) const;

};
