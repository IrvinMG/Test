// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFItemTypes.h"
#include "ARSTypes.h"
#include "CoreMinimal.h"
#include "Items/ACFItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ACFItemSystemFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYSYSTEM_API UACFItemSystemFunctionLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

  public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static AACFWorldItem *SpawnWorldItemNearLocation(UObject *WorldContextObject, const FBaseItem &ContainedItem, const FVector &location, float acceptanceRadius = 100.f);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool GetItemData(const TSubclassOf<class AACFItem> &item, FItemDescriptor &outData);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool GetEquippableAttributeSetModifier(const TSubclassOf<class AACFItem> &itemClass, FAttributesSetModifier &outModifier);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool GetEquippableAttributeRequirements(const TSubclassOf<class AACFItem> &itemClass, TArray<FAttribute> &outAttributes);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool GetConsumableTimedAttributeSetModifier(const TSubclassOf<class AACFItem> &itemClass, TArray<FTimedAttributeSetModifier> &outModifiers);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool GetConsumableStatModifier(const TSubclassOf<class AACFItem> &itemClass, TArray<FStatisticValue> &outModifiers);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetItemTypeTagRoot();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetItemSlotTagRoot();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool IsValidItemTypeTag(FGameplayTag TagToCheck);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool IsValidItemSlotTag(FGameplayTag TagToCheck);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FTransform GetShootTransform(APawn *SourcePawn, EShootTargetType targetType);

    UFUNCTION(BlueprintPure, Category = ACFLibrary)
    static TSubclassOf<class AACFWorldItem> GetDefaultWorldItemClass();

    UFUNCTION(BlueprintPure, Category = ACFLibrary)
    static void FilterByItemType(const TArray<FInventoryItem>& inItems, EItemType inType, TArray<FInventoryItem>& outItems);

    UFUNCTION(BlueprintPure, Category = ACFLibrary)
    static void FilterByItemSlot(const TArray<FInventoryItem>& inItems, FGameplayTag inSlot, TArray<FInventoryItem>& outItems);
};
