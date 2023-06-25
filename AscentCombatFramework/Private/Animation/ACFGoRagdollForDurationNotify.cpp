// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Animation/ACFGoRagdollForDurationNotify.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFRagdollComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UACFGoRagdollForDurationNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const AACFCharacter* acfChar = Cast<AACFCharacter>(MeshComp->GetOwner());
	if (acfChar) {
		acfChar->GetRagdollComponent()->GoRagdollForDuration(RagdollImpulse, Duration);
	}
}
