// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFItemsManagerComponent.h"
#include "Engine/DataTable.h"
#include "GameplayTagsManager.h"
#include "ACFCraftingComponent.h"
#include "Components/ACFEquipmentComponent.h"
#include "ACFCurrencyComponent.h"

// Sets default values for this component's properties
UACFItemsManagerComponent::UACFItemsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UACFItemsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}


bool UACFItemsManagerComponent::GenerateItemsFromRules(const TArray<FACFItemGenerationRule>& generationRules, TArray<FBaseItem>& outItems)
{
	outItems.Empty();
	bool bIsSuccessful = true;

	if (generationRules.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Missing generation rules! - UACFItemsManagerComponent"));
		return false;
	}

	for (auto genRule : generationRules) {
		FBaseItem outItem;
		if (GenerateItemFromRule(genRule, outItem)) {
			outItems.Add(outItem);
		}
		else {
			bIsSuccessful = false;
		}
	}
	return bIsSuccessful;
	
}

bool UACFItemsManagerComponent::GenerateItemFromRule(const FACFItemGenerationRule& generationRules, FBaseItem& outItem)
{
	TArray<FItemGenerationSlot> matchingItems;
	if (ItemsDB) {
		const auto items = ItemsDB->GetRowMap();
		for (const auto itemItr : items ){
			FItemGenerationSlot* itemSlot = (FItemGenerationSlot*)itemItr.Value;
			if (itemSlot) {
				if (DoesSlotMatchesRule(generationRules, *itemSlot)) {
					matchingItems.Add(*itemSlot);
				}
			} else {
				return false;
			}
		}

		if (matchingItems.Num() == 0) {
			UE_LOG(LogTemp, Warning, TEXT("No Matching Items in DB! - UACFItemsManagerComponent"));
			return false;
		}

		const int32 selectedSlot = FMath::RandRange(0, matchingItems.Num() - 1);
		const int32 selectedCount = FMath::RandRange(generationRules.MinItemCount, generationRules.MaxItemCount);
		if (matchingItems.IsValidIndex(selectedSlot) && selectedCount > 0) {
			const TSubclassOf<AACFItem> itemClass = matchingItems[selectedSlot].ItemClass.LoadSynchronous();
			if (itemClass) {
				outItem = FBaseItem(itemClass, selectedCount);
				return true;
			}		
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No  ItemsDB! in ItemsManager!!!!- UACFItemsManagerComponent"));
	}
	return false;
}

bool UACFItemsManagerComponent::DoesSlotMatchesRule(const FACFItemGenerationRule& generationRules, const FItemGenerationSlot& item)
{
	return (
		(item.Category == generationRules.Category || 
		UGameplayTagsManager::Get().RequestGameplayTagChildren(generationRules.Category).HasTag(item.Category))
		&&
		(item.Rarity == generationRules.Rarity ||
		UGameplayTagsManager::Get().RequestGameplayTagChildren(generationRules.Rarity).HasTag(item.Rarity))
	);
}

bool UACFItemsManagerComponent::TryGetRecipe(const FName& receiptKey, FACFCraftingRecipe& outRecipe)
{
	if (RecipesDB) {
		FACFCraftingRecipe* receipt = RecipesDB->FindRow<FACFCraftingRecipe>(receiptKey, "Finding receipt");
		if (receipt) {
			outRecipe = *receipt;
			return true;
		}
	}
	return false;  
}

/*------------------- SERVER SIDE -----------------------------------*/
// void UACFItemsManagerComponent::AddRecipe_Implementation(const FName& receiptKey, class UACFCraftingComponent* craftingComp)
// {
// 	FACFCraftingRecipe outRecipe;
// 	if (TryGetRecipe(receiptKey, outRecipe) && craftingComp) {
// 		craftingComp->AddNewRecipe(outRecipe);
// 	}
// }
// 
// bool UACFItemsManagerComponent::AddRecipe_Validate(const FName& receiptKey, class UACFCraftingComponent* craftingComp)
// {
// 	return true;
// }


void UACFItemsManagerComponent::SellItemsToVendor_Implementation(const FInventoryItem& itemTobeSold, APawn* instigator, int32 count /*= 1*/, class UACFCraftingComponent* craftingComp)
{
	if (!craftingComp) {
		return;
	}

	if (!craftingComp->CanPawnSellItemFromHisInventory(itemTobeSold, instigator, count)) {
		return;
	}

	UACFEquipmentComponent* equipComp = craftingComp->GetPawnEquipment(instigator);
	UACFCurrencyComponent* currencyComp = craftingComp->GetPawnCurrencyComponent(instigator);
	float totalCost = itemTobeSold.ItemInfo.CurrencyValue * count * craftingComp->GetVendorPriceMultiplierOnBuy();
	if (equipComp && currencyComp && craftingComp->GetVendorCurrencyComp()) {
		equipComp->RemoveItem(itemTobeSold, count);
		currencyComp->AddCurrency(totalCost);
		craftingComp->GetVendorCurrencyComp()->RemoveCurrency(totalCost);
		craftingComp->AddItem(FBaseItem(itemTobeSold.itemClass, count));
	}
}

bool UACFItemsManagerComponent::SellItemsToVendor_Validate(const FInventoryItem& itemTobeSold, APawn* instigator, int32 count /*= 1*/, class UACFCraftingComponent* craftingComp)
{
	return true;
}

void UACFItemsManagerComponent::BuyItems_Implementation(const FBaseItem& item, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	if (!craftingComp) {
		return;
	}

	if (!craftingComp->CanPawnBuyItems(item, instigator)) {
		return;
	}

	FItemDescriptor itemToBuyDesc;
	if (craftingComp->TryGetItemDescriptor(item, itemToBuyDesc)) {
		const float totalCost = (itemToBuyDesc.CurrencyValue * item.Count * craftingComp->GetVendorPriceMultiplierOnSell());
		UACFCurrencyComponent* currencyComp = craftingComp->GetPawnCurrencyComponent(instigator);
		UACFEquipmentComponent* equipComp = craftingComp->GetPawnEquipment(instigator);
		if (currencyComp && equipComp) {
			currencyComp->RemoveCurrency(totalCost);
			equipComp->AddItemToInventory(item);
			craftingComp->RemoveItem(item);
			if (craftingComp->GetVendorCurrencyComp()) {
				craftingComp->GetVendorCurrencyComp()->AddCurrency(totalCost);
			}
			return;
		}
	}
	return;
}

bool UACFItemsManagerComponent::BuyItems_Validate(const FBaseItem& item, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	return true;
}

bool UACFItemsManagerComponent::CraftItem_Validate(const FACFCraftingRecipe& ItemToCraft, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	return true;
}

void UACFItemsManagerComponent::CraftItem_Implementation(const FACFCraftingRecipe& ItemToCraft, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	if (!craftingComp) {
		return;
	}

	if (!craftingComp->CanPawnCraftItem(ItemToCraft, instigator)) {
		return;
	}
	UACFEquipmentComponent* equipComp = craftingComp->GetPawnEquipment(instigator);

	if (!equipComp) {
		return;
	}

	craftingComp->GetPawnCurrencyComponent(instigator)->RemoveCurrency(craftingComp->GetVendorPriceMultiplierOnSell() * ItemToCraft.CraftingCost);
	equipComp->ConsumeItems(ItemToCraft.RequiredItems);
	equipComp->AddItemToInventoryByClass(ItemToCraft.OutputItem.ItemClass, ItemToCraft.OutputItem.Count);
	return;
}

void UACFItemsManagerComponent::UpgradeItem_Implementation(const FInventoryItem& itemToUpgrade, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	if (!craftingComp) {
		return;
	}

	if (!craftingComp->CanPawnUpgradeItem(itemToUpgrade, instigator)) {
		return;
	}

	UACFEquipmentComponent* equipComp = craftingComp->GetPawnEquipment(instigator);
	UACFCurrencyComponent* currencyComp = craftingComp->GetPawnCurrencyComponent(instigator);
	if (equipComp && currencyComp) {
		FItemDescriptor itemInfo = itemToUpgrade.ItemInfo;

		if (itemInfo.NextLevelClass) {
			equipComp->ConsumeItems(itemInfo.RequiredItemsToUpgrade);
			currencyComp->RemoveCurrency(craftingComp->GetVendorPriceMultiplierOnSell() * itemInfo.UpgradeCurrencyCost);
			equipComp->RemoveItem(itemToUpgrade, 1);
			equipComp->AddItemToInventoryByClass(itemInfo.NextLevelClass, 1);
			return;
		}
	}
	return;
}

bool UACFItemsManagerComponent::UpgradeItem_Validate(const FInventoryItem& itemToUpgrade, APawn* instigator, class UACFCraftingComponent* craftingComp)
{
	return true;
}
