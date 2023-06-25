// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Animation/ACFNotifyAction.h"
#include "Components/ACFActionsManagerComponent.h"
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/KismetSystemLibrary.h>

void UACFNotifyAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
//     if (bActivateOnlyOnServer && !UKismetSystemLibrary::IsServer(this)) {
//         return;
//     }

    if (MeshComp && MeshComp->GetOwner()) {

        UACFActionsManagerComponent* _amc = MeshComp->GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
        if (_amc) {
            _amc->AnimationsReachedNotablePoint();
        }
    }
}
