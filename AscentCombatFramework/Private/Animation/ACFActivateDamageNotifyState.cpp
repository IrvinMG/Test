// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Animation/ACFActivateDamageNotifyState.h"
#include <Components/SkeletalMeshComponent.h>
#include "Actors/ACFCharacter.h"

void UACFActivateDamageNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp) {

		AACFCharacter* CharacterOwner = Cast< AACFCharacter>(MeshComp->GetOwner());
		if (CharacterOwner) {
			CharacterOwner->ActivateDamage(DamageToActivate, TraceChannels);
		}
	}

}

void UACFActivateDamageNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp) {

		AACFCharacter* CharacterOwner = Cast< AACFCharacter>(MeshComp->GetOwner());
		if (CharacterOwner) {
			CharacterOwner->DeactivateDamage(DamageToActivate, TraceChannels);
		}
	}
}
