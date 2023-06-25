// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ACFItem.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/ACFStorageComponent.h"
#include "ACFCraftingComponent.generated.h"



UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class CRAFTINGSYSTEM_API UACFCraftingComponent : public UACFStorageComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFCraftingComponent();

	/*------------------- CHECKS -----------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "ACF | Checks")
	bool CanPawnBuyItems(const FBaseItem& itemsToBuy, const APawn* buyer) const;

	UFUNCTION(BlueprintCallable, Category = "ACF | Checks")
	bool CanPawnCraftItem(const FACFCraftingRecipe& itemToCraft, const APawn* buyer)  const;

	UFUNCTION(BlueprintCallable, Category = "ACF | Checks")
	bool CanPawnSellItemFromHisInventory(const FInventoryItem& itemTobeSold, const APawn* seller, int32 count = 1) const;

	UFUNCTION(BlueprintCallable, Category = "ACF | Checks")
	bool CanPawnUpgradeItem(const FInventoryItem& itemToUpgrade, const APawn* pawnOwner) const;

	/*------------------- SERVER SIDE -----------------------------------*/

	UFUNCTION( BlueprintCallable, Category = "ACF | Vendor")
	void BuyItems(const FBaseItem& item, APawn* instigator);

	UFUNCTION( BlueprintCallable, Category = "ACF | Vendor")
    void SellItemsToVendor(const FInventoryItem&  itemTobeSold, APawn* instigator, int32 count = 1 );

	UFUNCTION(BlueprintCallable, Category = "ACF | Crafting")
	void CraftItem(const FACFCraftingRecipe& ItemToCraft, APawn* instigator);

	UFUNCTION( BlueprintCallable, Category = "ACF | Crafting")
	void UpgradeItem(const FInventoryItem& itemToUpgrade, APawn* instigator);

	/*-------------------PLAYER STUFF-----------------------------------*/

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	float GetPawnCurrency(const APawn* pawn) const;

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	class UACFEquipmentComponent* GetPawnEquipment(const APawn* pawn) const;

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	TArray<FInventoryItem> GetPawnInventory(const APawn* pawn) const;

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	class UACFCurrencyComponent* GetPawnCurrencyComponent(const APawn* pawn) const;

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	TArray<FInventoryItem> GetAllPawnUpgradableItems(const APawn* pawn) const;

	UFUNCTION(BlueprintPure,Category = "ACF | Getters")
	float GetVendorCurrency() const;

	UFUNCTION(BlueprintPure,  Category = "ACF | Getters")
	class UACFCurrencyComponent* GetVendorCurrencyComp() const {
		return sellerCurrency;
	}

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	float GetVendorPriceMultiplierOnBuy() const {
		return PriceMultiplierOnBuy;
	}

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	float GetVendorPriceMultiplierOnSell() const {
		return PriceMultiplierOnSell;
	}

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	class UACFItemsManagerComponent* GetItemsManager() const;

	UFUNCTION(BlueprintPure, Category = "ACF | Getters")
	TArray<FACFCraftingRecipe> GetCraftableRecipes() const{
		return CraftableItems;
	}

	UFUNCTION(BlueprintCallable, Category = "ACF | Getters")
	bool TryGetCraftableRecipeForItem(const FBaseItem& recipe, FACFCraftingRecipe& outRecipe) const {
		if (CraftableItems.Contains(recipe)) {
			outRecipe = *CraftableItems.FindByKey(recipe);
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "ACF | Setters")
	void AddNewRecipe(const  FACFCraftingRecipe& recipe) {
		CraftableItems.Add(recipe);
	}

	UFUNCTION(BlueprintCallable, Category = "ACF | Setters")
	void AddRecipe(const FName& recipeKey);

	/*------------------- FUNCTION LIBRAR WRAPPERS-----------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "ACF | Library")
	bool TryGetItemDescriptor(const FBaseItem& item, FItemDescriptor& outItemDescriptor) const;

	UFUNCTION(BlueprintCallable, Category = "ACF | Library")
	bool TryGetItemDescriptorByClass(const TSubclassOf<AACFItem>& ItemClass, FItemDescriptor& outItemDescriptor) const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*Items this vendor can craft*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TArray<FName> ItemsRecipes;

	/*The multiplier this vendor is going to apply to the item value when selling to the player*/
	/*With this set to 0 this can be used as a chest that can be gathered without removing currencies*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float PriceMultiplierOnSell = 1.f;

	/*The multiplier this vendor is going to apply to the item value when buying from the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float PriceMultiplierOnBuy = 0.2f;

private:

	UPROPERTY()
	UACFCurrencyComponent* sellerCurrency;

	UPROPERTY()
	TArray<FACFCraftingRecipe> CraftableItems;

};
