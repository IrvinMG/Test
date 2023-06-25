// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Animation/ACFActionSubState.h"
#include "Components/ACFActionsManagerComponent.h"
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/KismetSystemLibrary.h>

void UACFActionSubState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
// 	if (bActivateOnlyOnServer && !UKismetSystemLibrary::IsServer(this)) {
// 		return;
// 	}
    if (MeshComp && MeshComp->GetOwner()) {
        UACFActionsManagerComponent* _amc = MeshComp->GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
        if (_amc) {
            _amc->StartSubState();
        }
    }
}

void UACFActionSubState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
//     if (bActivateOnlyOnServer && !UKismetSystemLibrary::IsServer(this)) {
//         return;
//     }
    if (MeshComp && MeshComp->GetOwner()) {
        UACFActionsManagerComponent* _amc = MeshComp->GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();

        if (_amc) {
            _amc->EndSubState();
        }
    }
}
