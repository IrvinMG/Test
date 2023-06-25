// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "ACFCoreTypes.h"
#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include "ACFInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UACFInteractableInterface : public UInterface
{
	GENERATED_BODY()

};

/**
 * 
 */
class ASCENTCOREINTERFACES_API IACFInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	void OnInteractedByPawn(class APawn* Pawn, const FString& interactionType = "");

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	void OnInteractableRegisteredByPawn(class APawn* Pawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	void OnInteractableUnregisteredByPawn(class APawn* Pawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	FText GetInteractableName();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	bool CanBeInteracted(class APawn* Pawn);
	virtual bool CanBeInteracted_Implementation(class APawn* Pawn) {
		return true;
	}

};
