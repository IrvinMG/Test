// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFWorldItem.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/ACFStorageComponent.h"
#include "Items/ACFItem.h"
#include "Net/UnrealNetwork.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include "ACFItemSystemFunctionLibrary.h"

AACFWorldItem::AACFWorldItem()
{
    ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
    StorageComponent = CreateDefaultSubobject<UACFStorageComponent>(TEXT("StorageComponent"));
    bReplicates = true;
    /*	InteractableArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
    ObjectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AACFWorldItem::BeginPlay()
{
    Super::BeginPlay();
    if (GetItems().IsValidIndex(0)) {
        SetItemMesh(GetItems()[0]);
    }
    if (HasAuthority()) {
        StorageComponent->OnItemChanged.AddDynamic(this, &AACFWorldItem::HandleStorageChanged);
    }
   
}

void AACFWorldItem::SetItemMesh(const FBaseItem& inItem)
{
    if (ObjectMesh &&  inItem.ItemClass && StorageComponent) {
        if (UACFItemSystemFunctionLibrary::GetItemData(inItem.ItemClass, ItemInfo)) {
            
            if (ItemInfo.WorldMesh) {
                ObjectMesh->SetStaticMesh(ItemInfo.WorldMesh);
            } else {
                UE_LOG(LogTemp, Log, TEXT("StaticMesh Not Assigned!!! - ACFWorldItem"));
            }
        }
    } else {
        UE_LOG(LogTemp, Log, TEXT("Trying to assign a wrong Item to the World Item!! - ACFWorldItem"));
    }
}

void AACFWorldItem::AddItem(const FBaseItem& inItem)
{
    if (StorageComponent) {
        StorageComponent->AddItem(inItem);
        SetItemMesh(inItem);
    }
}

void AACFWorldItem::OnInteractedByPawn_Implementation(class APawn* Pawn, const FString& interactionType)
{
    if (Pawn) {
        UACFEquipmentComponent* equipComp = Pawn->FindComponentByClass<UACFEquipmentComponent>();
        if (equipComp) {
            StorageComponent->MoveItemsToInventory(GetItems(), equipComp);
        }
    }
}

void AACFWorldItem::OnInteractableRegisteredByPawn_Implementation(class APawn* Pawn)
{
}

void AACFWorldItem::OnInteractableUnregisteredByPawn_Implementation(class APawn* Pawn)
{
}

FText AACFWorldItem::GetInteractableName_Implementation()
{
    FString completeName = "";
    if (GetItems().IsValidIndex(0)) {
        completeName = ItemInfo.Name.ToString() + " x" + FString::FromInt(GetItems()[0].Count);
    }

    return FText::FromString(completeName);
}

bool AACFWorldItem::CanBeInteracted_Implementation(class APawn* Pawn)
{
    return true;
}

TArray<FBaseItem> AACFWorldItem::GetItems() const
{ 
    return StorageComponent->GetItems(); 
}

void AACFWorldItem::HandleStorageChanged_Implementation(const TArray<FBaseItem>& items)
{
    if (bDestroyOnGather && items.Num() == 0) {
        Destroy(true);
    }
}

void AACFWorldItem::OnLoaded_Implementation()
{
	if (GetItems().IsValidIndex(0)) {
		SetItemMesh(GetItems()[0]);
	}
}
