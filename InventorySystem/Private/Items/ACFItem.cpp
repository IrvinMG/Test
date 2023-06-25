// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Items/ACFItem.h"
#include "HAL/HideTCHAR.h"

// Sets default values
AACFItem::AACFItem()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    ItemInfo.ItemType = EItemType::Other;
}

void AACFItem::OnRep_ItemOwner()
{
}

void AACFItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AACFItem, ItemOwner);
}


