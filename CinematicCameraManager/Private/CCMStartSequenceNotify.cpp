// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "CCMStartSequenceNotify.h"
#include <Components/SkeletalMeshComponent.h>
#include "CCMCameraFunctionLibrary.h"
#include "CCMPlayerCameraManager.h"

void UCCMStartSequenceNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp) {
		AActor* actorOwner = MeshComp->GetOwner();

		ACCMPlayerCameraManager* cameraManager = UCCMCameraFunctionLibrary::GetLocalCinematicCameraManager(this);
		if (cameraManager) {
			cameraManager->TriggerCameraSequence(actorOwner, SequenceName);
		}
	}
}
