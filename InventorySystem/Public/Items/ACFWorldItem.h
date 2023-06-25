// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "Components/ACFEquipmentComponent.h"
#include "CoreMinimal.h"
#include "Interfaces/ACFInteractableInterface.h"
#include "Items/ACFItem.h"
#include "ALSSavableInterface.h"
#include "ACFWorldItem.generated.h"

class UACFStorageComponent;
class UStaticMeshComponent;

/**
 * 
 */

UCLASS()
class INVENTORYSYSTEM_API AACFWorldItem : public AActor, public IACFInteractableInterface, public IALSSavableInterface {
    GENERATED_BODY()

public:
    AACFWorldItem();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetItemMesh(const FBaseItem& inItem);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void AddItem(const FBaseItem& inItem);

    //INTERACTION INTERFACE
    /* called when player interact with object of this class */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnInteractedByPawn(class APawn* Pawn, const FString& interactionType = "");
    virtual void OnInteractedByPawn_Implementation(class APawn* Pawn, const FString& interactionType = "") override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnInteractableRegisteredByPawn(class APawn* Pawn);
    virtual void OnInteractableRegisteredByPawn_Implementation(class APawn* Pawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnInteractableUnregisteredByPawn(class APawn* Pawn);
    virtual void OnInteractableUnregisteredByPawn_Implementation(class APawn* Pawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool CanBeInteracted(class APawn* Pawn);
    virtual bool CanBeInteracted_Implementation(class APawn* Pawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    FText GetInteractableName();
    virtual FText GetInteractableName_Implementation() override;
    //END INTERACTION INTERFACE

    /*Return the items related to this world Item*/
    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<FBaseItem> GetItems() const; 

    /* Get the Item descriptor */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FItemDescriptor GetFirstItemInfo() const { return ItemInfo; }

    /* Destroy this actor when all items are gathered */
    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool DestroyOnAllItemsGathered() const { return bDestroyOnGather; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void HandleStorageChanged(const TArray<FBaseItem>& items);
    virtual void HandleStorageChanged_Implementation(const TArray<FBaseItem>& items);

protected:
    //changed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    bool bDestroyOnGather = true;

    /* Mesh to be displayed */
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ACF)
    TObjectPtr<UStaticMeshComponent> ObjectMesh;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ACF)
    TObjectPtr<UACFStorageComponent> StorageComponent;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FItemDescriptor ItemInfo;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ALS)
	void OnLoaded();
};
