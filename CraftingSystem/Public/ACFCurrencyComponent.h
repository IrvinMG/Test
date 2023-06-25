// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "ACFCurrencyComponent.generated.h"


UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class CRAFTINGSYSTEM_API UACFCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFCurrencyComponent();
	 
	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
	void AddCurrency(float amount);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
	void RemoveCurrency(float amount);

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool HasEnoughCurrency(float amount) const {
		return CurrencyAmount >= amount;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetCurrentCurrencyAmount() const {
		return CurrencyAmount;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(SaveGame, Replicated, EditDefaultsOnly, Category = ACF)
	float CurrencyAmount = 0.f;

	/*If set to true, if the entity owner is killed by a Player controlled character,
	all the currency will be added to that controller*/
	UPROPERTY( EditDefaultsOnly, Category = ACF)
	bool bDropCurrencyToPlayerKillerOnOwnerDeath = true;

private: 

	UFUNCTION()
	void HandleStatReachedZero(FGameplayTag stat);
	
};
