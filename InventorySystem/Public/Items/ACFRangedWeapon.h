// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "Components/ACFShootingComponent.h"
#include "CoreMinimal.h"
#include "Items/ACFWeapon.h"
/*#include "Game/ACFTypes.h"*/
#include "ACFRangedWeapon.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYSYSTEM_API AACFRangedWeapon : public AACFWeapon {
    GENERATED_BODY()

public:
    AACFRangedWeapon();

    /*Shoots at owner's eyesight*/
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    virtual void Shoot(const FRotator& deltaRot = FRotator::ZeroRotator, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

    /*Shoots at the target actor*/
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    virtual void ShootAtActor(const AActor* target, float randomDeviation = 5.f, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

    /*Shoots at the provided direction */
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void ShootAtDirection(const FRotator& direction, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

    /*Tries to swipe shoot following the targetType rule*/
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void SwipeTraceShoot(EShootTargetType targetType = EShootTargetType::CameraTowardsFocus);

    /*Tries to shoot the provided projectile or the one equipped in the Ammo slot following the targetType rule*/
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void ShootProjectile(EShootTargetType targetType = EShootTargetType::WeaponTowardsFocus, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACF)
    void Reload(bool bTryToEquipAmmo);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetProjectileSpeed() const
    {
        return ShootingComp ? ShootingComp->GetProjectileSpeed() : -1.f;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetAmmoMagazine() const
    {
        return ShootingComp->GetAmmoMagazine();
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetCurrentAmmo() const
    {
        return ShootingComp->GetCurrentAmmoInMagazine();
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FVector GetShootingSocket() const
    {
        return ShootingComp->GetShootingSocketPosition();
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EShootingType GetShootingType() const
    {
        return ShootingType;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE UACFShootingComponent* GetShootingComponent() const
    {
        return ShootingComp;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool CanShoot() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool NeedsReload() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = ACF)
    TObjectPtr<class UACFShootingComponent> ShootingComp;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ACF | Weapon|Ranged")
    EShootingType ShootingType = EShootingType::EProjectile;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ACF | Weapon|Ranged")
    bool TryEquipAmmos = true;

    virtual void OnRep_ItemOwner() override;

    void InitShooting();

    void OnItemEquipped_Implementation() override;

private:
};
