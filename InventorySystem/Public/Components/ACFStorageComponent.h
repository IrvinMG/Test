// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Items/ACFItem.h"
#include "ACFItemTypes.h"
#include "ACFStorageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemsChanged, const TArray<FBaseItem>&, currentItems);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UACFStorageComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFStorageComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, ReplicatedUsing = OnRep_Items, Category = ACF)
    TArray<FBaseItem> Items;

public:
   

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void RemoveItems(const TArray<FBaseItem>& inItems);

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void RemoveItem(const FBaseItem& inItems);

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void AddItem(const FBaseItem& inItems);

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void AddItems(const TArray<FBaseItem>& inItems);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void MoveItemsToInventory(const TArray<FBaseItem>& inItems, UACFEquipmentComponent* equipComp);

	/*Return the items related to this world Item*/
	UFUNCTION(BlueprintPure, Category = ACF)
    TArray<FBaseItem> GetItems() const {
        return Items;
    }

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnItemsChanged OnItemChanged;

private:
    UFUNCTION()
    void OnRep_Items();
};
