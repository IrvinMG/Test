// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFShootingComponent.h"
#include "ACMCollisionManagerComponent.h"
#include "ACMCollisionsFunctionLibrary.h"
#include "ACMTypes.h"
#include "Components/ACFEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "Items/ACFProjectile.h"
#include "Net/UnrealNetwork.h"
#include <Engine/World.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>

// Sets default values for this component's properties
UACFShootingComponent::UACFShootingComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    ProjectileShotSpeed = 3500.0f;
    SetIsReplicatedByDefault(true);
}

void UACFShootingComponent::SetupShootingComponent_Implementation(class APawn* inOwner, class UMeshComponent* inMesh)
{
    shootingMesh = inMesh;
    characterOwner = inOwner;
    Internal_SetupComponent(inOwner, inMesh);
}

bool UACFShootingComponent::SetupShootingComponent_Validate(class APawn* inOwner, class UMeshComponent* inMesh)
{
    return true;
}

void UACFShootingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFShootingComponent, characterOwner);
    DOREPLIFETIME(UACFShootingComponent, shootingMesh);
    DOREPLIFETIME(UACFShootingComponent, currentMagazine);
}

// Called when the game starts
void UACFShootingComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UACFShootingComponent::ShootAtDirection(const FRotator& direction, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride)
{
    if (!CanShoot()) {
        return;
    }

    const FVector ShotDirection = direction.Vector();

    FTransform spawnTransform;
    const FVector startingPos = GetShootingSocketPosition();
    spawnTransform.SetLocation(startingPos);
    spawnTransform.SetRotation(direction.Quaternion());
    spawnTransform.SetScale3D(FVector(1.f, 1.f, 1.f));

    Internal_Shoot(spawnTransform, ShotDirection, charge, projectileOverride);
}

void UACFShootingComponent::SwipeTraceShootAtDirection(const FVector& start, const FVector& direction)
{
    if (!CanShoot()) {
        return;
    }

    UACMCollisionManagerComponent* collisionMan = GetOwner()->FindComponentByClass<UACMCollisionManagerComponent>();
    if (collisionMan) {
        const FVector endTrace = start + (direction * ShootRange);
        collisionMan->PerformSwipeTraceShot(start, endTrace, ShootRadius);
        PlayMuzzleEffect();
        RemoveAmmo();
    }
}

void UACFShootingComponent::ReduceAmmoMagazine_Implementation(int32 amount /*= 1*/)
{
    currentMagazine -= amount;
    if (currentMagazine < 0) {
        currentMagazine = 0;
    }
}

void UACFShootingComponent::Reload_Implementation(bool bTryToEquipAmmo = true)
{
    FEquippedItem equip;
    bool bFoundAmmo = false;

    // let's see if we have equipped the ammo
    if (bTryToEquipAmmo) {
        if (TryGetAmmoSlot(equip)) {
            const AACFProjectile* projectileClass = Cast < AACFProjectile>(equip.InventoryItem.itemClass->GetDefaultObject());
            if (projectileClass && CanUseProjectile(projectileClass->GetClass())) {
                bFoundAmmo = true; 
            } else {
                bFoundAmmo = TryEquipAmmoFromInventory();
            }

        } else {
            bFoundAmmo = TryEquipAmmoFromInventory();
        }
    } else {
        bFoundAmmo = TryGetAmmoSlot(equip);
    }

    // if we have valid ammos, we refill the magazine
    if (bFoundAmmo) {
        currentMagazine = equip.InventoryItem.Count > AmmoMagazine ? AmmoMagazine : equip.InventoryItem.Count;
        OnCurrentAmmoChanged.Broadcast(GetCurrentAmmoInMagazine(), GetCurrentAmmoCount());
    }
}

bool UACFShootingComponent::TryEquipAmmoFromInventory()
{
    FInventoryItem invItem;
    FEquippedItem equip;

    // otherwise we try to equip it from the inventory
    if (TryGetAllowedAmmoFromInventory(invItem)) {
        UACFEquipmentComponent* equipComp = TryGetEquipment();
        equipComp->EquipItemFromInventory(invItem);
        return TryGetAmmoSlot(equip);
    }
    return false;
}

bool UACFShootingComponent::CanShoot() const
{
    if (!(shootingMesh && characterOwner)) {
        UE_LOG(LogTemp, Error, TEXT("Shooting component not initialized! - UACFShootingComponent"));
        return false;
    }

    if (!bConsumeAmmo) {
        return true;
    } else {
        FEquippedItem equip;
        if (TryGetAmmoSlot(equip)) {
            const AACFProjectile* projectileClass = Cast<AACFProjectile>(equip.InventoryItem.itemClass->GetDefaultObject());
            if (bUseMagazine) {             
                return CanUseProjectile(projectileClass->GetClass()) && currentMagazine > 0;
            } else {
                return CanUseProjectile(projectileClass->GetClass());
            }
        } else {
            UE_LOG(LogTemp, Warning, TEXT("No Ammo Slot in Equipment! - UACFShootingComponent::"));
        }
    }
    return false;
}

bool UACFShootingComponent::CanUseProjectile(const TSubclassOf<AACFProjectile>& projectileClass) const
{
    return AllowedProjectiles.Contains(projectileClass);
}

bool UACFShootingComponent::NeedsReload() const
{
    FEquippedItem equip;
    if (bUseMagazine && currentMagazine <= 0) {
        return TryGetAmmoSlot(equip);
    }
    return false;
}

bool UACFShootingComponent::TryGetAllowedAmmoFromInventory(FInventoryItem& outAmmoSlot) const
{
    UACFEquipmentComponent* equipComp = TryGetEquipment();
    outAmmoSlot.Count = 0;
    bool bFound = false;

    if (equipComp) {
        FEquippedItem currentSlot;
        const bool bHasCurrentSlot = TryGetAmmoSlot(currentSlot);

        // first we trywith current item class
        if (bHasCurrentSlot && AllowedProjectiles.Contains(currentSlot.InventoryItem.itemClass)) {
            TArray<FInventoryItem> invItems;
            equipComp->GetAllItemsOfClassInInventory(currentSlot.InventoryItem.itemClass, invItems);
            for (const auto& item : invItems) {
                if (outAmmoSlot.Count < item.Count) {
                    outAmmoSlot = item;
                    bFound = true;
                }
            }
        }
        // if none, we try with all the possible classes
        else {
            for (const auto& projClass : AllowedProjectiles) {
                TArray<FInventoryItem> invItems;
                equipComp->GetAllItemsOfClassInInventory(projClass, invItems);
                for (const auto& item : invItems) {
                    if (outAmmoSlot.Count < item.Count) {
                        outAmmoSlot = item;
                        bFound = true;
                    }
                }
            }
        }
    }
    return bFound;
}

void UACFShootingComponent::ShootAtActor(const AActor* target, float randomDeviation /*= 50.f*/, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride)
{
    if (!CanShoot()) {
        return;
    }

    UWorld* world = GetWorld();

    if (target && ProjectileClassBP) {
        const FVector SpawnProjectileLocation = shootingMesh->GetSocketLocation(ProjectileStartSocket);
        const FRotator ProjectileOrientation = shootingMesh->GetSocketRotation(ProjectileStartSocket);

        const FVector targetLocation = target->GetActorLocation();

        FVector FlyDir;
        float FlyDistance;
        (targetLocation - SpawnProjectileLocation).ToDirectionAndLength(FlyDir, FlyDistance);
        const float FlyTime = FlyDistance / ProjectileShotSpeed;
        FVector PredictedPosition = targetLocation + (target->GetVelocity() * FlyTime);

        const float RandomOffset_z = FMath::RandRange(-randomDeviation, randomDeviation);
        const float RandomOffset_x = FMath::RandRange(-randomDeviation, randomDeviation);
        const float RandomOffset_y = FMath::RandRange(-randomDeviation, randomDeviation);

        // Override Predicted Location with offset value
        PredictedPosition += FVector(RandomOffset_x, RandomOffset_y, RandomOffset_z);

        // Finalize Projectile fly direction
        FlyDir = (PredictedPosition - SpawnProjectileLocation).GetSafeNormal();

        FTransform spawnTransform;
        spawnTransform.SetLocation(SpawnProjectileLocation);
        spawnTransform.SetRotation(ProjectileOrientation.Quaternion());

        Internal_Shoot(spawnTransform, FlyDir, charge, projectileOverride);
    }
}

void UACFShootingComponent::OnRep_currentMagazine()
{
    OnCurrentAmmoChanged.Broadcast(currentMagazine, AmmoMagazine);
}

void UACFShootingComponent::Internal_Shoot(const FTransform& spawnTransform, const FVector& ShotDirection, float charge, TSubclassOf<class AACFProjectile> projectileOverride)
{
    if (!CanShoot()) {
        return;
    }

    TSubclassOf<class AACFItem> projToSpawn;
    if (projectileOverride) {
        projToSpawn = projectileOverride;
    } else {
        projToSpawn = GetBestProjectileToShoot();
    }

    UWorld* world = GetWorld();
    AACFProjectile* projectile = world->SpawnActorDeferred<AACFProjectile>(projToSpawn,
        spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    projectile->SetupProjectile(characterOwner);
    projectile->FinishSpawning(spawnTransform);
    projectile->ActivateDamage();
    if (projectile) {
        projectile->GetProjectileMovementComp()->Velocity = ShotDirection * ProjectileShotSpeed * charge;
    }

    PlayMuzzleEffect();
    RemoveAmmo();
}

void UACFShootingComponent::Internal_SetupComponent_Implementation(class APawn* inOwner, class UMeshComponent* inMesh)
{
    shootingMesh = inMesh;
    characterOwner = inOwner;
}

bool UACFShootingComponent::Internal_SetupComponent_Validate(class APawn* inOwner, class UMeshComponent* inMesh)
{
    return true;
}

UACFEquipmentComponent* UACFShootingComponent::TryGetEquipment() const
{
    if (characterOwner) {
        return characterOwner->FindComponentByClass<UACFEquipmentComponent>();
    }
    return nullptr;
}

bool UACFShootingComponent::TryGetAmmoSlot(FEquippedItem& outSlot) const
{
    UACFEquipmentComponent* equipCom = TryGetEquipment();
    if (equipCom) {
        return equipCom->GetEquippedItemSlot(AmmoSlot, outSlot);
    }
    return false;
}

void UACFShootingComponent::RemoveAmmo()
{
    if (bConsumeAmmo) {
        UACFEquipmentComponent* equipCom = TryGetEquipment();
        if (equipCom) {
            FEquippedItem equip;
            if (equipCom->GetEquippedItemSlot(AmmoSlot, equip)) {
                equipCom->RemoveItem(equip.InventoryItem, 1);
                if (bUseMagazine) {
                    ReduceAmmoMagazine(1);
                }
                OnCurrentAmmoChanged.Broadcast(GetCurrentAmmoInMagazine(), GetCurrentAmmoCount());

            }
        }
    }
}

TSubclassOf<AACFItem> UACFShootingComponent::GetBestProjectileToShoot() const
{
    if (bConsumeAmmo) {
        FEquippedItem item;
        if (TryGetAmmoSlot(item)) {
            return (item.InventoryItem.itemClass);
        }
    } else {
        return ProjectileClassBP;
    }
    return nullptr;
}

int32 UACFShootingComponent::GetTotalEquippedAmmoCount() const
{
    FEquippedItem item;
    if (TryGetAmmoSlot(item)) {
        return item.InventoryItem.Count;
    }
    return 0;
}

int32 UACFShootingComponent::GetCurrentAmmoCount() const
{
    FEquippedItem currentSlot;
    const bool bHasCurrentSlot = TryGetAmmoSlot(currentSlot);
    if (bHasCurrentSlot) {
        return currentSlot.InventoryItem.Count;
    }
    return 0;
}

void UACFShootingComponent::PlayMuzzleEffect_Implementation()
{
    if (shootingMesh) {
        const FVector MuzzleLocation = shootingMesh->GetSocketLocation(ProjectileStartSocket);
        const FRotator MuzzleRotation = shootingMesh->GetSocketRotation(ProjectileStartSocket);

        FImpactFX FxToPlay = FImpactFX(ShootingEffect);
        FxToPlay.SpawnLocation.SetLocation(FxToPlay.SpawnLocation.GetLocation() + MuzzleLocation);
        FxToPlay.SpawnLocation.SetRotation(FxToPlay.SpawnLocation.GetRotation() + MuzzleRotation.Quaternion());
        UACMCollisionsFunctionLibrary::PlayEffectLocally(FxToPlay, this);
    }
}
