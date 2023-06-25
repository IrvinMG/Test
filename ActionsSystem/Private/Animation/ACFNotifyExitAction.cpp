// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Animation/ACFNotifyExitAction.h"
#include <Components/SkeletalMeshComponent.h>
#include "Components/ACFActionsManagerComponent.h"

void UACFNotifyExitAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (MeshComp && MeshComp->GetOwner()) {
        UACFActionsManagerComponent* _amc = MeshComp->GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
        if (_amc) {
            _amc->FreeAction();
        }
    }
}
