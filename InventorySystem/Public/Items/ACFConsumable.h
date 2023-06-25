// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFItem.h"
#include "ACMTypes.h"
#include "ARSTypes.h"
#include "CoreMinimal.h"

#include "ACFConsumable.generated.h"

/**
 *
 */

UCLASS()
class INVENTORYSYSTEM_API AACFConsumable : public AACFItem {
    GENERATED_BODY()

    AACFConsumable();

    friend class UACFEquipmentComponent;

  public:
    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<FStatisticValue> GetStatsModifiers() const { return StatModifier; }

    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<FTimedAttributeSetModifier> GetTimedModifiers() const { return TimedAttributeSetModifier; }

  protected:
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnItemUsed();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Consumable")
    TArray<FStatisticValue> StatModifier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Consumable")
    TArray<FTimedAttributeSetModifier> TimedAttributeSetModifier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF | Consumable")
    FActionEffect OnUsedEffect;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ACF | Consumable")
    bool bConsumeOnUse = true;

  private:
    void Internal_UseItem(class ACharacter *owner);
};
