// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ACFEquippableItem.h"
#include <Components/StaticMeshComponent.h>

#include "ACFProjectile.generated.h"


UCLASS()
class INVENTORYSYSTEM_API AACFProjectile : public AACFEquippableItem {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AACFProjectile();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovementComp() { return ProjectileMovementComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UStaticMesh* GetStaticMesh() { return MeshComp->GetStaticMesh(); }

    FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComp; };

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACMCollisionManagerComponent* GetCollisionComp() const { return CollisionComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool ShouldBeDroppedOnDeath() const
    {
        return bDroppableWhenAttached;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetDropOnDeathPercentage() const
    {
        return DropRatePercentage;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetupProjectile(class APawn* inOwner);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ActivateDamage();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Internal_OnEquipped(class ACharacter* _owner) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USphereComponent> SphereComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UACMCollisionManagerComponent> CollisionComp;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ACF | Projectile")
    float ProjectileLifespan = 5.f;

    /* If this projectile must remain attached to the body of the character hit*/
    UPROPERTY(EditDefaultsOnly, Category = "ACF | Projectile")
    bool bAttachOnHit = false;

    /* How deep this projectile must penetrate  the body of the character hit*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bAttachOnHit"), Category = "ACF | Projectile")
    float PenetrationLevel = 30.f;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (EditCondition = "bAttachOnHit"), Category = "ACF | Projectile")
    float AttachedLifespan = 10.f;

    /* If this is set to true when this projectile is attached to an acfcharacter and
    this character dies, the projectile can be dropped as world item*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bAttachOnHit"), Category = "ACF | Projectile")
    bool bDroppableWhenAttached = true;

    /* When attached to an adversary, the chance that this item will be dropped when it dies*/
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bAttachOnHit"), Category = "ACF | Projectile")
    float DropRatePercentage = 100.f;

private:
    bool bIsFlying;

    void MakeStatic();

    UFUNCTION()
    void HandleAttackHit(const FHitResult& HitResult);
};
