// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "RootMotionModifier.h"
#include "Game/ACFTypes.h"
#include "ACFAttackAction.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFAttackAction : public UACFBaseAction
{
	GENERATED_BODY()

public: 

	UACFAttackAction();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE uint8 GetComboCounter() { return CurrentComboIndex; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ResetComboCounter() {
		CurrentComboIndex = 0;
	}

protected:

	virtual void OnActionStarted_Implementation(const FString& contextString = "") override;

	virtual void OnActionEnded_Implementation() override;

	virtual FName GetMontageSectionName_Implementation() override;

	virtual void OnSubActionStateEntered_Implementation() override;

	virtual void OnSubActionStateExited_Implementation() override;

	virtual void OnActionTransition_Implementation(class UACFBaseAction* previousState) override;

	virtual FTransform GetWarpTransform_Implementation() override;

	virtual USceneComponent* GetWarpTargetComponent_Implementation() override;

	virtual void OnTick_Implementation(float DeltaTime) override;

	uint8 CurrentComboIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	EDamageActivationType DamageToActivate;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TArray<FName> TraceChannels;

	UPROPERTY(EditDefaultsOnly, Category = "ACF| Warp")
	bool bCheckWarpConditions = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bCheckWarpConditions), Category = "ACF| Warp")
	float maxWarpDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bCheckWarpConditions), Category = "ACF| Warp")
	float minWarpDistance = 10.f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bCheckWarpConditions), Category = "ACF| Warp")
	float maxWarpAngle = 270.f;

	UPROPERTY(EditDefaultsOnly, Category = "ACF| Warp")
    bool bContinuousUpdate = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bContinuousUpdate), Category = "ACF| Warp")
    float WarpMagnetismStrength = 1.0f;

	bool TryGetTransform(FTransform& outTranform) const;

private :

	float oldRotationRate;

	bool bSuccesfulCombo = false;

	FTransform warpTrans;
	USceneComponent* currentTargetComp;
};
