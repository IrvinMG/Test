// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFStorageComponent.h"
#include "Items/ACFItem.h"
#include "Components/ACFEquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFStorageComponent::UACFStorageComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
    // ...
}

// Called when the game starts
void UACFStorageComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UACFStorageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UACFStorageComponent, Items);
}

void UACFStorageComponent::RemoveItems_Implementation(const TArray<FBaseItem>& inItems)
{
    TArray<FBaseItem> pendingRemove;
    for (auto& item : inItems) {
        FBaseItem* currentItem = Items.FindByKey(item);

        if (currentItem) {
            currentItem->Count -= item.Count;

            if (currentItem->Count <= 0) {
                pendingRemove.Add(*currentItem);
            }
        }
    }

    for (auto& removed : pendingRemove) {
        Items.Remove(removed);
    }

    OnItemChanged.Broadcast(Items);
}

bool UACFStorageComponent::RemoveItems_Validate(const TArray<FBaseItem>& inItems)
{
    return true;
}


void UACFStorageComponent::RemoveItem_Implementation(const FBaseItem& inItem)
{
    FBaseItem* currentItem = Items.FindByKey(inItem);
    if (currentItem) {
        currentItem->Count -= inItem.Count;
        if (currentItem->Count <= 0) {
            Items.Remove(*currentItem);
        }
    }
    OnItemChanged.Broadcast(Items);
}

bool UACFStorageComponent::RemoveItem_Validate(const FBaseItem& inItems)
{
    return true;
}

void UACFStorageComponent::AddItems_Implementation(const TArray<FBaseItem>& inItems)
{
    for (const auto& item : inItems) {
        AddItem(item);
    }
}

bool UACFStorageComponent::AddItems_Validate(const TArray<FBaseItem>& inItems)
{
    return true;
}

void UACFStorageComponent::AddItem_Implementation(const FBaseItem& inItem)
{
    FBaseItem* currentItem = Items.FindByKey(inItem);

    if (currentItem) {
        currentItem->Count += inItem.Count;
    } else {
        Items.Add(inItem);
    }
    OnItemChanged.Broadcast(Items);
}

bool UACFStorageComponent::AddItem_Validate(const FBaseItem& inItems)
{
    return true;
}

void UACFStorageComponent::MoveItemsToInventory(const TArray<FBaseItem>& inItems, UACFEquipmentComponent* equipComp)
{
    if (equipComp) {
        equipComp->MoveItemsToInventory(inItems, this);
    }

}

//addition code
void UACFStorageComponent::OnRep_Items()
{
    OnItemChanged.Broadcast(Items);
}
