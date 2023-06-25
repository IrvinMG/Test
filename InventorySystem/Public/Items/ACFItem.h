// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFItemTypes.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>

#include "ACFItem.generated.h"

USTRUCT(BlueprintType)
struct FBaseItem : public FTableRowBase {
    GENERATED_BODY()

public:
    FBaseItem() {};

    FBaseItem(TSubclassOf<class AACFItem> inItem, uint8 inCount)
    {
        ItemClass = inItem;
        Count = inCount;
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ACF)
    TSubclassOf<class AACFItem> ItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ACF)
    int32 Count = 1;

    FORCEINLINE bool operator==(const FBaseItem& Other) const
    {
        return this->ItemClass == Other.ItemClass;
    }

    FORCEINLINE bool operator!=(const FBaseItem& Other) const
    {
        return this->ItemClass != Other.ItemClass;
    }

    FORCEINLINE bool operator!=(const TSubclassOf<class AACFItem>& Other) const
    {
        return this->ItemClass != Other;
    }

    FORCEINLINE bool operator==(const TSubclassOf<class AACFItem>& Other) const
    {
        return this->ItemClass == Other;
    }
};

USTRUCT(BlueprintType)
struct FACFCraftingRecipe : public FTableRowBase {
    GENERATED_BODY()

public:
    FACFCraftingRecipe() {};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<FBaseItem> RequiredItems;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FBaseItem OutputItem;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float CraftingCost = 0.f;

    FORCEINLINE bool operator!=(const FBaseItem& Other) const
    {
        return this->OutputItem.ItemClass != Other.ItemClass;
    }

    FORCEINLINE bool operator==(const FBaseItem& Other) const
    {
        return this->OutputItem.ItemClass == Other.ItemClass;
    }

    FORCEINLINE bool operator!=(const TSubclassOf<class AACFItem>& Other) const
    {
        return this->OutputItem.ItemClass != Other;
    }

    FORCEINLINE bool operator==(const TSubclassOf<class AACFItem>& Other) const
    {
        return this->OutputItem.ItemClass == Other;
    }

    FORCEINLINE bool operator!=(const FACFCraftingRecipe& Other) const
    {
        return this->OutputItem.ItemClass != Other.OutputItem.ItemClass;
    }

    FORCEINLINE bool operator==(const FACFCraftingRecipe& Other) const
    {
        return this->OutputItem.ItemClass == Other.OutputItem.ItemClass;
    }
};

USTRUCT(BlueprintType)
struct FItemDescriptor : public FTableRowBase {
    GENERATED_BODY()

public:
    FItemDescriptor()
    {
        ThumbNail = nullptr;
        WorldMesh = nullptr;
        GameSpecificData = nullptr;
        ItemType = EItemType::Other;
    };

    /*Icon to be displayed in UI*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    class UTexture2D* ThumbNail;

    /*Name of the item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    FText Name;

    /*Long description of the item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    FText Description;

    /*Type of item*/
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    EItemType ItemType;

    /*Decide the amount of stackable units when this item is in inventory*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    uint8 MaxInventoryStack = 1;

    /*Weight of this item in the inventory*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    float ItemWeight = 5.0;

    /*mesh to be used when spawned in world as world mesh, not mandatory*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    UStaticMesh* WorldMesh;

    /* If the item is droppable in the world*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    bool bDroppable = true;

    /* If this item can be upgraded*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    bool bUpgradable = false;

    /* The items required to upgrade this item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUpgradable == true"), Category = ACF)
    TArray<FBaseItem> RequiredItemsToUpgrade;

    /* Next level of this item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUpgradable == true"), Category = ACF)
    TSubclassOf<AACFItem> NextLevelClass;

    /*If upgrading this item has a cost in currency*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    float UpgradeCurrencyCost = 0.f;

    /*base price for selling / buying this item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    float CurrencyValue = 5.0;

    /*Slots in which this item can be equipped*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    TArray<FGameplayTag> ItemSlots;

    TArray<FGameplayTag> GetPossibleItemSlots() const { return ItemSlots; }

    /*Usable to add game specific data to any item*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACF)
    UPrimaryDataAsset* GameSpecificData;
};

UCLASS()
class INVENTORYSYSTEM_API AACFItem : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AACFItem();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UTexture2D* GetThumbnailImage() const { return ItemInfo.ThumbNail; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FText GetItemName() const { return ItemInfo.Name; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FText GetItemDescription() const { return ItemInfo.Description; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EItemType GetItemType() { return ItemInfo.ItemType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class APawn* GetItemOwner() { return ItemOwner; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FItemDescriptor GetItemInfo() const { return ItemInfo; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE TArray<FGameplayTag> GetPossibleItemSlots() const { return ItemInfo.GetPossibleItemSlots(); }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetItemDescriptor(const FItemDescriptor& itemDesc)
    {
        ItemInfo = itemDesc;
    }

    void SetItemOwner(APawn* inOwner)
    {
        ItemOwner = inOwner;
    }
private:
protected:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ItemOwner, Category = ACF)
    class APawn* ItemOwner;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ACF | Item")
    FItemDescriptor ItemInfo;

    UFUNCTION()
    virtual void OnRep_ItemOwner();
};
