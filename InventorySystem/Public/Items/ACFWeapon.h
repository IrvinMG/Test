// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFEquippableItem.h"

#include "ACFWeapon.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FWeaponEffects {
    GENERATED_BODY()

public:
    FWeaponEffects()
    {
        AttackSound = nullptr;
        AttackParticle = nullptr;
    }
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    class USoundCue* AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    class UParticleSystem* AttackParticle;
};

UCLASS()
class INVENTORYSYSTEM_API AACFWeapon : public AACFEquippableItem {
    GENERATED_BODY()

public:
    friend class UACFEquipmentComponent;

    AACFWeapon();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EHandleType GetHandleType() const { return HandleType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetWeaponType() const { return WeaponType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetAssociatedMovesetTag() const { return Moveset; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetAssociatedMovesetActionsTag() const { return MovesetActions; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetAssociatedMovesetOverlayTag() const { return MovesetOverlay; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FName GetOnBodySocketName() const { return OnBodySocketName; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FName GetEquippedSocketName() const { return InHandsSocketName; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool OverridesMainHandMoveset() const { return bOverrideMainHandMoveset; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool OverridesMainHandOverlay() const { return bOverrideMainHandOverlay; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool OverridesMainHandMovesetActions() const { return bOverrideMainHandMovesetActions; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetHandleType(EHandleType newType) { HandleType = newType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsUsingLeftHandIK() const { return bUseLeftHandIKPosition; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FVector GetLeftHandleIKPosition() const { return LeftHandleIKPos->GetComponentLocation(); }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetUnsheatedAttributeSetModifier(const FAttributesSetModifier& inAttributeModifier) { UnsheathedAttributeModifier = inAttributeModifier; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FAttributesSetModifier GetUnsheatedAttributeSetModifier() const { return UnsheathedAttributeModifier; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    FORCEINLINE class USkeletalMeshComponent* GetMeshComponent() const { return Mesh; };

protected:
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "ACF | Weapon")
    TObjectPtr<class USceneComponent> HandlePos;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "ACF | Weapon")
    TObjectPtr<class USceneComponent> LeftHandleIKPos;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACMCollisionManagerComponent> CollisionComp;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Weapon")
    EHandleType HandleType = EHandleType::OneHanded;

    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "HandleType == EHandleType::OffHand"), EditDefaultsOnly, Category = "ACF | Weapon")
    bool bOverrideMainHandMoveset = false;

    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "HandleType == EHandleType::OffHand"), EditDefaultsOnly, Category = "ACF | Weapon")
    bool bOverrideMainHandMovesetActions = false;

    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "HandleType == EHandleType::OffHand"), EditDefaultsOnly, Category = "ACF | Weapon")
    bool bOverrideMainHandOverlay = false;

    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "HandleType == EHandleType::TwoHanded"), EditDefaultsOnly, Category = "ACF | Weapon")
    bool bUseLeftHandIKPosition = false;

    virtual void OnItemEquipped_Implementation() override;

    virtual void OnItemUnEquipped_Implementation() override;

    /*Tag identifying the actual weapontype*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FTransform AttachmentOffset;

    /*Tag identifying the actual weapontype*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FGameplayTag WeaponType;

    /*Once this weapon is equipped, the equipping character will try to switch to this moveset*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FGameplayTag Moveset;

    /*Once this weapon is equipped, the equipping character will try to add this overlay*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FGameplayTag MovesetOverlay;

    /*Once this weapon is equipped, those are the actions that can be triggered*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FGameplayTag MovesetActions;

    /*Socket in which this weapon will be attached once it is equipped on the back of the character*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FName OnBodySocketName;

    /*Socket in which this weapon will be attached once it is unsheathed*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Weapon")
    FName InHandsSocketName;

    UPROPERTY()
    FGameplayTag CurrentEnchantment;

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnWeaponUnsheathed();
    virtual void OnWeaponUnsheathed_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnWeaponSheathed();
    virtual void OnWeaponSheathed_Implementation();

    /*Modifier applied once this item is equipped*/
    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = "ACF | Weapon")
    FAttributesSetModifier UnsheathedAttributeModifier;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ACF)
    TObjectPtr<class USkeletalMeshComponent> Mesh;

private:
    UFUNCTION()
    void Internal_OnWeaponUnsheathed();

    UFUNCTION()
    void Internal_OnWeaponSheathed();

private:
    void AlignWeapon();
};
