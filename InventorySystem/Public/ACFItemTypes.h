// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Engine/DataTable.h"
#include "ACFItemTypes.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHandleType : uint8 {
	OneHanded = 0 UMETA(DisplayName = "One Hand"),
	OffHand = 1 UMETA(DisplayName = "Off-Hand Weapon"),
	TwoHanded = 2 UMETA(DisplayName = "Two Handed Weapon"),
};

/** Defines how to the shoot trace is handled */
UENUM(BlueprintType)
enum class EShootTargetType : uint8
{
	// From the player's camera towards camera focus
	CameraTowardsFocus,
	// From the pawn's center, in the pawn's orientation
	PawnForward,
	// From the pawn's center, oriented towards camera focus
	PawnTowardsFocus,
	// From the weapon's muzzle or location, in the pawn's orientation
	WeaponForward,
	// From the weapon's muzzle or location, towards camera focus
	WeaponTowardsFocus,
};

UENUM(BlueprintType)
enum class EItemType : uint8 {
    Armor = 0 UMETA(DisplayName = "Armor"),
    MeleeWeapon = 1 UMETA(DisplayName = "Melee Weapon"),
    RangedWeapon = 2 UMETA(DisplayName = "RangedWeapon"),
    Consumable = 3 UMETA(DisplayName = "Consumable"),
    Material = 4 UMETA(DisplayName = "Material"),
    Accessory = 5 UMETA(DisplayName = "Accessory"),
    Projectile = 6 UMETA(DisplayName = "Ammo"),
    Other = 7 UMETA(DisplayName = "Other"),
};

USTRUCT(BlueprintType)
struct FModularPart {
    GENERATED_BODY()

public:
    FModularPart() {
        meshComp = nullptr;
    };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FGameplayTag ItemSlot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    class USkeletalMeshComponent* meshComp;

    FORCEINLINE bool operator==(const FModularPart& Other) const
    {
        return this->ItemSlot == Other.ItemSlot;
    }

    FORCEINLINE bool operator!=(const FModularPart& Other) const
    {
        return this->ItemSlot != Other.ItemSlot;
    }

    FORCEINLINE bool operator!=(const FGameplayTag& Other) const
    {
        return this->ItemSlot != Other;
    }

    FORCEINLINE bool operator==(const FGameplayTag& Other) const
    {
        return this->ItemSlot == Other;
    }
};

USTRUCT(BlueprintType)
struct FItemGenerationSlot : public FTableRowBase {
	GENERATED_BODY()

public:
    FItemGenerationSlot() {
    };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TSoftClassPtr<class AACFItem> ItemClass;


};

USTRUCT(BlueprintType)
struct FACFItemGenerationRule {
	GENERATED_BODY()

public:
    FACFItemGenerationRule() {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	int32 MinItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	int32 MaxItemCount = 1;
};



UENUM(BlueprintType)
enum class EShootingType : uint8
{
	EProjectile        UMETA(DisplayName = "Shoot Projectile"),
	ESwipeTrace		  UMETA(DisplayName = "Shoot Swipe Trace"),
};



UCLASS()
class INVENTORYSYSTEM_API UACFItemTypes : public UObject {
    GENERATED_BODY()
};
