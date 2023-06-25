// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFEquippableItem.h"
#include "ACFArmor.generated.h"

/**
 * 
 */
class USkeletalMeshComponent;

UCLASS()
class INVENTORYSYSTEM_API AACFArmor : public AACFEquippableItem
{
	GENERATED_BODY()

public:

	AACFArmor();

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	 USkinnedAsset* GetArmorMesh() const; 


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<USkeletalMeshComponent> MeshComp;
	
};
