// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ACFItemTypes.h"
#include "Items/ACFItem.h"
#include "Components/ACFEquipmentComponent.h"
#include "ACFItemsManagerComponent.generated.h"



UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class CRAFTINGSYSTEM_API UACFItemsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFItemsManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
	UDataTable* ItemsDB;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
	UDataTable* RecipesDB;

public:	

	/* Generates an array of FBaseItem matching the provided rules by selecting them from the provided ItemsDB
	returns true only if we are able to find matching items for ALL the provided rules*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	bool GenerateItemsFromRules(const TArray<FACFItemGenerationRule>& generationRules, TArray<FBaseItem>& outItems); 

	/* Generates an  FBaseItem matching the provide rule by selecting it from the provided ItemsDB
    returns true if at least one item is found*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	bool GenerateItemFromRule(const FACFItemGenerationRule& generationRules, FBaseItem& outItems);

	/* Returns true if the provided itemSlot matches the provided slot rules*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	bool DoesSlotMatchesRule(const FACFItemGenerationRule& generationRules,  const FItemGenerationSlot& item);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryGetRecipe(const FName& receiptKey, FACFCraftingRecipe& outRecipe);


	/*------------------- SERVER SIDE -----------------------------------*/

// 	UFUNCTION(Server, WithValidation, Reliable)
// 	void AddRecipe(const FName& receiptKey, class UACFCraftingComponent* craftingComp);

	UFUNCTION(Server, WithValidation, Reliable)
	void BuyItems(const FBaseItem& item, APawn* instigator, class UACFCraftingComponent* craftingComp);

	UFUNCTION(Server, WithValidation, Reliable)
	void SellItemsToVendor(const FInventoryItem& itemTobeSold, APawn* instigator, int32 count, class UACFCraftingComponent* craftingComp);

	UFUNCTION(Server, WithValidation, Reliable)
	void CraftItem(const FACFCraftingRecipe& ItemToCraft, APawn* instigator, class UACFCraftingComponent* craftingComp);

	UFUNCTION(Server, WithValidation, Reliable)
	void UpgradeItem(const FInventoryItem& itemToUpgrade, APawn* instigator, class UACFCraftingComponent* craftingComp);
		
};
