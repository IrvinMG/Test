// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Animation/ACFNotifyFX.h"
#include "Components/ACFActionsManagerComponent.h"
#include <Animation/AnimSequenceBase.h>
#include <Components/SkeletalMeshComponent.h>

void UACFNotifyFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (MeshComp && MeshComp->GetOwner()) {
        UACFActionsManagerComponent* amc = MeshComp->GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
        if (amc) {
            amc->PlayCurrentActionFX();
        }
    }
}
