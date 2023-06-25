// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFEquippableItem.h"
#include "ARSStatisticsComponent.h"
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>

AACFEquippableItem::AACFEquippableItem()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AACFEquippableItem::Internal_OnEquipped(ACharacter* charOwner)
{
    if (charOwner) {
        ItemOwner = charOwner;
        SetOwner(ItemOwner);
        if (EquipSound) {
            UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
        }
        AddModifierToOwner(AttributeModifier);
        OnItemEquipped();
    } else {
        UE_LOG(LogTemp, Error, TEXT("Invalid owner -AACFEquippableItem "));
    }
}

void AACFEquippableItem::RemoveModifierToOwner(const FAttributesSetModifier& inModifier)
{
    if (ItemOwner) {
        UARSStatisticsComponent* statcomp = ItemOwner->FindComponentByClass<UARSStatisticsComponent>();
        if (statcomp) {
            statcomp->RemoveAttributeSetModifier(inModifier);
        }
    }
}

void AACFEquippableItem::AddModifierToOwner(const FAttributesSetModifier& inModifier)
{
    if (ItemOwner) {
        UARSStatisticsComponent* statcomp = ItemOwner->FindComponentByClass<UARSStatisticsComponent>();
        if (statcomp) {
            statcomp->AddAttributeSetModifier(inModifier);
        }
    }
}

void AACFEquippableItem::Internal_OnUnEquipped()
{
    RemoveModifierToOwner(AttributeModifier);

    if (UnequipSound) {
        UGameplayStatics::PlaySoundAtLocation(this, UnequipSound, GetActorLocation());
    }
    OnItemUnEquipped();
}

void AACFEquippableItem::OnItemEquipped_Implementation()
{
}

void AACFEquippableItem::OnItemUnEquipped_Implementation()
{
}
