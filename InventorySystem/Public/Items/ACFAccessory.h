// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFEquippableItem.h"
#include "ACFAccessory.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AACFAccessory : public AACFEquippableItem
{
	GENERATED_BODY()

public:

	AACFAccessory();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FName GetAttachmentSocket() const {
		return AttachmentSocket;
	}

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	FName AttachmentSocket;
	
};
