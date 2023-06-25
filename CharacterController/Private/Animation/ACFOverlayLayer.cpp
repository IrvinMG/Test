// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 


#include "Animation/ACFOverlayLayer.h"
#include "ACFCCFunctionLibrary.h"
#include "Components/ACFCharacterMovementComponent.h"


void UACFOverlayLayer::SetReferences()
{
	const APawn* pawn = TryGetPawnOwner();
	if (pawn) {
		MovementComp = Cast<UACFCharacterMovementComponent>(pawn->GetMovementComponent());
		if (!MovementComp) {
			UE_LOG(LogTemp, Error, TEXT("Owner doesn't have ACFCharachterMovement Comp - UACFOverlayLayer::SetReferences!!!!"));
			SetMovStance(MovementComp->GetCurrentMovementStance());
		}
	}
}




void UACFOverlayLayer::SetMovStance(const EMovementStance inOverlay)
{
	currentOverlay = inOverlay;
	switch (currentOverlay) {
		case EMovementStance::EIdle:
			OverlayBlendAlfa = IdleOverlay.BlendAlpha;
			break;
		case EMovementStance::EAiming:
			OverlayBlendAlfa = AimOverlay.BlendAlpha;
			break;
		case EMovementStance::EBlock:
			OverlayBlendAlfa = BlockOverlay.BlendAlpha;
			break;
		case EMovementStance::ECustom:
			OverlayBlendAlfa = CustomOverlay.BlendAlpha;
			break;
		default:
			OverlayBlendAlfa = IdleOverlay.BlendAlpha;
			break;
	 }
}

void UACFOverlayLayer::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SetReferences();
}

void UACFOverlayLayer::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!MovementComp) {
		SetReferences();
	}
	else {
		const EMovementStance newOverlay =  MovementComp->GetCurrentMovementStance();
		//if (newOverlay != currentOverlay) {
			SetMovStance(newOverlay);
		//}
		
	}
}
