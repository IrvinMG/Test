// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ACFAnimTypes.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "ACFCCTypes.h"
#include "ACFAnimLayer.h"
#include "ACFOverlayLayer.generated.h"

/**
 * 
 */
 class UAnimSequence;
UCLASS()
 class CHARACTERCONTROLLER_API UACFOverlayLayer : public UACFAnimLayer
 {
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | AimOffset")
	UAimOffsetBlendSpace* AimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Overlay")
	FOverlayConfig IdleOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Overlay")
	FOverlayConfig AimOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Overlay")
	FOverlayConfig BlockOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Overlay")
	FOverlayConfig CustomOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "ACF | Overlay")
	float OverlayBlendAlfa = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Overlay")
	float DefaultSwitchTime = 0.7f;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class UACFCharacterMovementComponent* MovementComp;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	EMovementStance currentOverlay;

	void SetReferences();

	void SetMovStance(const EMovementStance inOverlay);

	/* begin play */
	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
};
