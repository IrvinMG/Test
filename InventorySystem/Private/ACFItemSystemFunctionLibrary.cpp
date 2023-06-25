// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACFItemSystemFunctionLibrary.h"
#include "ACFInventorySettings.h"
#include "AIController.h"
#include "Components/ACFEquipmentComponent.h"
#include "GameplayTagsManager.h"
#include "Items/ACFConsumable.h"
#include "Items/ACFEquippableItem.h"
#include "Items/ACFItem.h"
#include "Items/ACFRangedWeapon.h"
#include "Items/ACFWorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "UObject/SoftObjectPtr.h"

AACFWorldItem *UACFItemSystemFunctionLibrary::SpawnWorldItemNearLocation(UObject *WorldContextObject, const FBaseItem &ContainedItem, const FVector &location,
                                                                         float acceptanceRadius /*= 100.f*/) {
    FVector outLoc;

    if (ContainedItem.Count <= 0) {
        return nullptr;
    }
    if (UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(WorldContextObject, location, outLoc, acceptanceRadius)) {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        TSubclassOf<AACFWorldItem> WorldItemClass = UACFItemSystemFunctionLibrary::GetDefaultWorldItemClass();
        ensure(WorldItemClass);

        APlayerController *playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
        UWorld *world = playerController->GetWorld();

        if (!world) {
            return nullptr;
        }

        if (world && WorldItemClass) {
            AACFWorldItem *_worldItem = world->SpawnActor<AACFWorldItem>(WorldItemClass, outLoc, FRotator(0), SpawnParams);
            if (_worldItem) {
                _worldItem->AddItem(ContainedItem);
                return _worldItem;
            }
        } else {
            UE_LOG(LogTemp, Error, TEXT("Impossible to Spawn Item!! MISSING NAVMESH - UACFFunctionLibrary::SpawnWorldItemNearLocation"));
        }
    }
    return nullptr;
}

bool UACFItemSystemFunctionLibrary::GetItemData(const TSubclassOf<class AACFItem> &item, FItemDescriptor &outData) {
    /*	item.LoadSynchronous();*/
    if (item) {
        const AACFItem *itemInstance = Cast<AACFItem>(item.Get()->GetDefaultObject());
        if (itemInstance) {
            outData = itemInstance->GetItemInfo();
            return true;
        }
    }
    return false;
}

bool UACFItemSystemFunctionLibrary::GetEquippableAttributeSetModifier(const TSubclassOf<class AACFItem> &itemClass, FAttributesSetModifier &outModifier) {
    /*	itemClass.LoadSynchronous();*/
    if (itemClass) {
        const AACFEquippableItem *itemInstance = Cast<AACFEquippableItem>(itemClass.GetDefaultObject());
        if (itemInstance) {
            outModifier = itemInstance->GetAttributeSetModifier();
            return true;
        }
    }
    return false;
}

bool UACFItemSystemFunctionLibrary::GetEquippableAttributeRequirements(const TSubclassOf<class AACFItem> &itemClass, TArray<FAttribute> &outAttributes) {
    /*	itemClass.LoadSynchronous();*/
    if (itemClass) {
        const AACFEquippableItem *itemInstance = Cast<AACFEquippableItem>(itemClass.GetDefaultObject());
        if (itemInstance) {
            outAttributes = itemInstance->GetAttributeRequirement();
            return true;
        }
    }
    return false;
}

bool UACFItemSystemFunctionLibrary::GetConsumableTimedAttributeSetModifier(const TSubclassOf<class AACFItem> &itemClass, TArray<FTimedAttributeSetModifier> &outModifiers) {
    /*	itemClass.LoadSynchronous();*/
    if (itemClass) {
        const AACFConsumable *itemInstance = Cast<AACFConsumable>(itemClass.GetDefaultObject());
        if (itemInstance) {
            outModifiers = itemInstance->GetTimedModifiers();
            return true;
        }
    }
    return false;
}

bool UACFItemSystemFunctionLibrary::GetConsumableStatModifier(const TSubclassOf<class AACFItem> &itemClass, TArray<FStatisticValue> &outModifiers) {
    /*itemClass.LoadSynchronous();*/
    if (itemClass) {
        const AACFConsumable *itemInstance = Cast<AACFConsumable>(itemClass.GetDefaultObject());
        if (itemInstance) {
            outModifiers = itemInstance->GetStatsModifiers();
            return true;
        }
    }
    return false;
}

FGameplayTag UACFItemSystemFunctionLibrary::GetItemTypeTagRoot() {
    UACFInventorySettings *settings = GetMutableDefault<UACFInventorySettings>();

    if (settings) {
        return settings->ItemtypeTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - UACFItemSystemFunctionLibrary "));

    return FGameplayTag();
}

FGameplayTag UACFItemSystemFunctionLibrary::GetItemSlotTagRoot() {
    UACFInventorySettings *settings = GetMutableDefault<UACFInventorySettings>();

    if (settings) {
        return settings->ItemSlotsTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - UACFItemSystemFunctionLibrary "));

    return FGameplayTag();
}

TSubclassOf<AACFWorldItem> UACFItemSystemFunctionLibrary::GetDefaultWorldItemClass() {
    UACFInventorySettings *settings = GetMutableDefault<UACFInventorySettings>();

    if (settings) {
        return settings->WorldItemClass;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Default Item Class! - UACFFunctionLibrary "));

    return nullptr;
}

void UACFItemSystemFunctionLibrary::FilterByItemType(const TArray<FInventoryItem>& inItems, EItemType inType, TArray<FInventoryItem>& outItems)
{
    outItems.Empty();

    for (const FInventoryItem& item : inItems) {
        if (item.ItemInfo.ItemType == inType) {
            outItems.Add(item);
        }
    }
}

void UACFItemSystemFunctionLibrary::FilterByItemSlot(const TArray<FInventoryItem>& inItems, FGameplayTag inSlot, TArray<FInventoryItem>& outItems)
{
    outItems.Empty();

    for (const FInventoryItem& item : inItems) {
        if (item.ItemInfo.ItemSlots.Contains(inSlot)) {
            outItems.Add(item);
        }
    }
}

bool UACFItemSystemFunctionLibrary::IsValidItemTypeTag(FGameplayTag TagToCheck) {
    const FGameplayTag root = GetItemTypeTagRoot();
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UACFItemSystemFunctionLibrary::IsValidItemSlotTag(FGameplayTag TagToCheck) {
    const FGameplayTag root = GetItemSlotTagRoot();
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

FTransform UACFItemSystemFunctionLibrary::GetShootTransform(APawn *SourcePawn, EShootTargetType targetType) {
    
    check(SourcePawn);

    UACFEquipmentComponent* equipComp =  SourcePawn->FindComponentByClass<UACFEquipmentComponent>();

    check(equipComp);
    const FVector ActorLoc = SourcePawn->GetActorLocation();
    FQuat AimQuat = SourcePawn->GetActorQuat();
    AController *Controller = SourcePawn->Controller;
    FVector SourceLoc;

    double FocalDistance = 1024.0f;
    FVector FocalLoc;

    FVector CamLoc;
    FRotator CamRot;
    bool bFoundFocus = false;

    if ((Controller) &&
        ((targetType == EShootTargetType::CameraTowardsFocus) || (targetType == EShootTargetType::PawnTowardsFocus) || (targetType == EShootTargetType::WeaponTowardsFocus))) {

        // Get camera position for later
        bFoundFocus = true;

        APlayerController *PC = Cast<APlayerController>(Controller);
        if (PC) {
            PC->GetPlayerViewPoint(/*out*/ CamLoc, /*out*/ CamRot);
        } else {
            SourceLoc = equipComp->GetMainWeaponSocketLocation();
            CamLoc = SourceLoc;
            CamRot = Controller->GetControlRotation();
        }

        // Determine initial focal point to
        FVector AimDir = CamRot.Vector().GetSafeNormal();
        FocalLoc = CamLoc + (AimDir * FocalDistance);

        // Move the start and focal point up in front of pawn
        if (PC) {
            const FVector WeaponLoc = equipComp->GetMainWeaponSocketLocation();
            CamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);
            FocalLoc = CamLoc + (AimDir * FocalDistance);
        }
        // Move the start to be the HeadPosition of the AI
        else if (AAIController *AIController = Cast<AAIController>(Controller)) {
            CamLoc = SourcePawn->GetActorLocation() + FVector(0, 0, SourcePawn->BaseEyeHeight);
        }

        if (targetType == EShootTargetType::CameraTowardsFocus) {
            // If we're camera -> focus then we're done
            return FTransform(CamRot, CamLoc);
        }
    }

    if ((targetType == EShootTargetType::WeaponForward) || (targetType == EShootTargetType::WeaponTowardsFocus)) {
        SourceLoc = equipComp->GetMainWeaponSocketLocation();
    } else {
        // Either we want the pawn's location, or we failed to find a camera
        SourceLoc = ActorLoc;
    }

    if (bFoundFocus && ((targetType == EShootTargetType::PawnTowardsFocus) || (targetType == EShootTargetType::WeaponTowardsFocus))) {
        // Return a rotator pointing at the focal point from the source
        return FTransform((FocalLoc - SourceLoc).Rotation(), SourceLoc);
    }

    // If we got here, either we don't have a camera or we don't want to use it, either way go forward
    return FTransform(AimQuat, SourceLoc);
}
