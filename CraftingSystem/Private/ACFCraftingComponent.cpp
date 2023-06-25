// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFCraftingComponent.h"
#include "ACFCurrencyComponent.h"
#include "ACFItemsManagerComponent.h"
#include "Components/ACFEquipmentComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "ACFItemSystemFunctionLibrary.h"

// Sets default values for this component's properties
UACFCraftingComponent::UACFCraftingComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UACFCraftingComponent::BeginPlay()
{
    Super::BeginPlay();

    sellerCurrency = GetOwner()->FindComponentByClass<UACFCurrencyComponent>();
    if (!sellerCurrency) {
        UE_LOG(LogTemp, Warning, TEXT("Seller with No Currencies! - UACFCraftingComponent::BeginPlay"));
    }

    for (const FName& receiptKey : ItemsRecipes) {
        AddRecipe(receiptKey);
    }
}

void UACFCraftingComponent::AddRecipe(const FName& recipeKey)
{
    UACFItemsManagerComponent* itemMan = GetItemsManager();
    if (!itemMan) {
        UE_LOG(LogTemp, Error, TEXT("Remember To Add Item Manager Component to your Game State!!!! - UACFCraftingComponent::BeginPlay"));
        return;
    }
    FACFCraftingRecipe outRecipe;
    if (itemMan->TryGetRecipe(recipeKey, outRecipe) ) {
        AddNewRecipe(outRecipe);
    }
}

/*------------------- CHECKS -----------------------------------*/

bool UACFCraftingComponent::CanPawnBuyItems(const FBaseItem& itemsToBuy, const APawn* buyer) const
{
    if (!Items.Contains(itemsToBuy.ItemClass)) {
        return false;
    }
    const FBaseItem* tobeSold = Items.FindByKey(itemsToBuy.ItemClass);
    if (tobeSold && tobeSold->Count >= itemsToBuy.Count) {
        if (PriceMultiplierOnSell == 0.f) {
            return true;
        }
        FItemDescriptor itemToBuyDesc;
        if (TryGetItemDescriptor(itemsToBuy, itemToBuyDesc)) {
            return (itemToBuyDesc.CurrencyValue * itemsToBuy.Count * PriceMultiplierOnSell) <= GetPawnCurrency(buyer);
        }
    }

    return false;
}

bool UACFCraftingComponent::CanPawnCraftItem(const FACFCraftingRecipe& itemToCraft, const APawn* buyer) const
{
    UACFEquipmentComponent* equipComp = GetPawnEquipment(buyer);

    if (equipComp->NumberOfItemCanTake(itemToCraft.OutputItem.ItemClass) < itemToCraft.OutputItem.Count) {
        return false;
    }

    return equipComp->HasEnoughItemsOfType(itemToCraft.RequiredItems) && GetPawnCurrency(buyer) >= PriceMultiplierOnSell * itemToCraft.CraftingCost;
}

bool UACFCraftingComponent::CanPawnSellItemFromHisInventory(const FInventoryItem& itemTobeSold, const APawn* seller, int32 count /*= 1*/) const
{
    UACFEquipmentComponent* equipComp = GetPawnEquipment(seller);
    if (!sellerCurrency || !equipComp) {
        return false;
    }

    ensure(false);
    return itemTobeSold.Count >= count && itemTobeSold.ItemInfo.CurrencyValue * count * PriceMultiplierOnBuy <= sellerCurrency->GetCurrentCurrencyAmount();
}

bool UACFCraftingComponent::CanPawnUpgradeItem(const FInventoryItem& itemToUpgrade, const APawn* pawnOwner) const
{
    if (!GetPawnInventory(pawnOwner).Contains(itemToUpgrade)) {
        return false;
    }

    UACFEquipmentComponent* equipComp = GetPawnEquipment(pawnOwner);

    if (equipComp) {
        if (itemToUpgrade.ItemInfo.bUpgradable && GetPawnCurrency(pawnOwner) >= PriceMultiplierOnSell * itemToUpgrade.ItemInfo.UpgradeCurrencyCost) {
            return equipComp->HasEnoughItemsOfType(itemToUpgrade.ItemInfo.RequiredItemsToUpgrade);
        }
    }
    return false;
}

/* ----------- TO SERVER---------------*/

void UACFCraftingComponent::BuyItems(const FBaseItem& item, APawn* instigator)
{
    if (GetItemsManager()) {
        GetItemsManager()->BuyItems(item, instigator, this);
    }
}

void UACFCraftingComponent::SellItemsToVendor(const FInventoryItem& itemTobeSold, APawn* instigator, int32 count /*= 1 */)
{
    if (GetItemsManager()) {
        GetItemsManager()->SellItemsToVendor(itemTobeSold, instigator, count, this);
    }
}

void UACFCraftingComponent::CraftItem(const FACFCraftingRecipe& ItemToCraft, APawn* instigator)
{
    if (GetItemsManager()) {
        GetItemsManager()->CraftItem(ItemToCraft, instigator, this);
    }
}

void UACFCraftingComponent::UpgradeItem(const FInventoryItem& itemToUpgrade, APawn* instigator)
{
    if (GetItemsManager()) {
        GetItemsManager()->UpgradeItem(itemToUpgrade, instigator, this);
    }
}

/*-------------------PLAYER STUFF-----------------------------------*/

float UACFCraftingComponent::GetPawnCurrency(const APawn* pawn) const
{
    if (!pawn) {
        return -1.f;
    }

    const UACFCurrencyComponent* currencyComp = GetPawnCurrencyComponent(pawn);
    if (currencyComp) {
        return currencyComp->GetCurrentCurrencyAmount();
    }
    return -1.f;
}

class UACFEquipmentComponent* UACFCraftingComponent::GetPawnEquipment(const APawn* pawn) const
{
    if (!pawn) {
        return nullptr;
    }

    return pawn->FindComponentByClass<UACFEquipmentComponent>();
}

TArray<FInventoryItem> UACFCraftingComponent::GetPawnInventory(const APawn* pawn) const
{
    const UACFEquipmentComponent* equipComp = GetPawnEquipment(pawn);
    if (equipComp) {
        return equipComp->GetInventory();
    }
    return TArray<FInventoryItem>();
}

class UACFCurrencyComponent* UACFCraftingComponent::GetPawnCurrencyComponent(const APawn* pawn) const
{
    if (!pawn) {
        return nullptr;
    }

    UACFCurrencyComponent* currencyComp = pawn->FindComponentByClass<UACFCurrencyComponent>();
    if (currencyComp) {
        return currencyComp;
    }
    AController* pawnContr = pawn->GetController();
    if (pawnContr) {
        UACFCurrencyComponent* currency = pawnContr->FindComponentByClass<UACFCurrencyComponent>();
        if (currency) {
            return currency;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Add Currency Component to your Player Controller! - UACFVendorComponent::GetPlayerEquipment  "));

    return nullptr;
}

TArray<FInventoryItem> UACFCraftingComponent::GetAllPawnUpgradableItems(const APawn* pawn) const
{
    TArray<FInventoryItem> upgradables;

    const TArray<FInventoryItem> playerInventory = GetPawnInventory(pawn);

    for (const auto& elem : playerInventory) {
        if (elem.ItemInfo.bUpgradable) {
            upgradables.Add(elem);
        }
    }
    return upgradables;
}

float UACFCraftingComponent::GetVendorCurrency() const
{
    if (sellerCurrency) {
        return sellerCurrency->GetCurrentCurrencyAmount();
    }
    return -1.f;
}

bool UACFCraftingComponent::TryGetItemDescriptor(const FBaseItem& item, FItemDescriptor& outItemDescriptor) const
{
    if (item.ItemClass) {
        return TryGetItemDescriptorByClass(item.ItemClass, outItemDescriptor);
    }
    return false;
}

bool UACFCraftingComponent::TryGetItemDescriptorByClass(const TSubclassOf<AACFItem>& ItemClass, FItemDescriptor& outItemDescriptor) const
{
    return UACFItemSystemFunctionLibrary::GetItemData(ItemClass, outItemDescriptor);
}

class UACFItemsManagerComponent* UACFCraftingComponent::GetItemsManager() const
{
    const APlayerController* gameState = UGameplayStatics::GetPlayerController(this, 0);
    if (gameState) {
        return gameState->FindComponentByClass<UACFItemsManagerComponent>();
    }
    return nullptr;
}
