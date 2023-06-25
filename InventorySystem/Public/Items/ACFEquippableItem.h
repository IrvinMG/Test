// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFItem.h"
#include "ARSTypes.h"
#include <Components/SkeletalMeshComponent.h>
#include "ACFEquippableItem.generated.h"

/**
 * 
 */


UCLASS()
class INVENTORYSYSTEM_API AACFEquippableItem : public AACFItem
{
	GENERATED_BODY()

public:

	friend class UACFEquipmentComponent;

	AACFEquippableItem();


	/*Played when equipped*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Equippable")
	class USoundCue* EquipSound;

	/*Played when unequipped*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | Equippable")
	class USoundCue* UnequipSound;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FAttributesSetModifier GetAttributeSetModifier() const {
		return AttributeModifier;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE TArray<FAttribute> GetAttributeRequirement() const {
		return PrimaryAttributesRequirement;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetAttributeSetModifier(const FAttributesSetModifier& inAttributeModifier) {
		AttributeModifier = inAttributeModifier;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetAttributesRequirement(const TArray<FAttribute>& inAttributeReq) {
		PrimaryAttributesRequirement = inAttributeReq;
	}

protected:

	void RemoveModifierToOwner(const FAttributesSetModifier& inModifier);
	void AddModifierToOwner(const FAttributesSetModifier& inModifier);


	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnItemEquipped();
	virtual void OnItemEquipped_Implementation();
		
			
	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnItemUnEquipped();
	virtual void OnItemUnEquipped_Implementation();


	/*Modifier applied once this item is equipped*/
	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = "ACF | Equippable")
	FAttributesSetModifier AttributeModifier;

	/*Attributes requirement to equip this item*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Equippable")
	TArray<FAttribute> PrimaryAttributesRequirement;

protected: 

	virtual void Internal_OnEquipped(class ACharacter* _owner);

	virtual void Internal_OnUnEquipped();
};
