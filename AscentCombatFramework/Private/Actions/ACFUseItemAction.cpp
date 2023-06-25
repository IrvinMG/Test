// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Actions/ACFUseItemAction.h"
#include "ACFItemTypes.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFEquipmentComponent.h"
#include "Items/ACFRangedWeapon.h"
#include "Items/ACFWeapon.h"

void UACFUseItemAction::OnActionStarted_Implementation(const FString& contextString)
{
    bSuccess = false;
    if (bCheckHandsIK) {
        const AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

        if (acfCharacter) {
            const UACFEquipmentComponent* equipComp = acfCharacter->GetEquipmentComponent();
            UACFAnimInstance* animInst = acfCharacter->GetACFAnimInstance();
            if (equipComp && animInst) {
                animInst->SetEnableHandIK(false);
            }
        }
    }
}

void UACFUseItemAction::OnNotablePointReached_Implementation()
{
    UseItem();
    bSuccess = true;
}

void UACFUseItemAction::OnActionEnded_Implementation()
{
    if (!bSuccess && bShouldUseIfInterrupted) {
        UseItem();
    }
    if (bCheckHandsIK) {
        const AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

        if (acfCharacter) {
            const UACFEquipmentComponent* equipComp = acfCharacter->GetEquipmentComponent();
            UACFAnimInstance* animInst = acfCharacter->GetACFAnimInstance();
            if (equipComp && animInst) {
                animInst->SetEnableHandIK(equipComp->ShouldUseLeftHandIK());
            }
        }
    }
}

UACFUseItemAction::UACFUseItemAction()
{
    ActionConfig.bPerformableInAir = true;
}

void UACFUseItemAction::UseItem()
{
    AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        UACFEquipmentComponent* equipComp = acfCharacter->GetEquipmentComponent();
        if (equipComp) {
            equipComp->UseEquippedItemBySlot(ItemSlot);
            if (bTryToEquipOffhand) {
                const AACFWeapon* mainWeap = equipComp->GetCurrentMainWeapon();
                if (mainWeap && /*mainWeap->GetItemSlot() == ItemSlot &&*/
                    mainWeap->GetHandleType() == EHandleType::OneHanded) {
                    equipComp->UseEquippedItemBySlot(OffHandSlot);
                }
            }
            if (bTryToEquipAmmo) {
                AACFRangedWeapon* rangedWeap = Cast<AACFRangedWeapon>(equipComp->GetCurrentMainWeapon());
                if (rangedWeap) {
                    rangedWeap->Reload(bTryToEquipAmmo);
                }
            }
        }
    }
}

bool UACFUseItemAction::CanExecuteAction_Implementation(class ACharacter* owner)
{
    AACFCharacter* acfCharacter = Cast<AACFCharacter>(owner);

    if (!acfCharacter)
        return false;

    const UACFEquipmentComponent* equipComp = acfCharacter->GetEquipmentComponent();

    if (equipComp) {
        return equipComp->HasAnyItemInEquipmentSlot(ItemSlot);
    }
    return false;
}
