// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Items/ACFRangedWeapon.h"
#include "ACFItemSystemFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Items/ACFItem.h"
#include "Items/ACFProjectile.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Sound/SoundCue.h>

AACFRangedWeapon::AACFRangedWeapon()
{
    ItemInfo.ItemType = EItemType::RangedWeapon;
    ItemInfo.Name = FText::FromString("Base Ranged Weapon");
    ShootingComp = CreateDefaultSubobject<UACFShootingComponent>(TEXT("ShotingComponent"));
}

void AACFRangedWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AACFRangedWeapon::OnRep_ItemOwner()
{
    InitShooting();
}

void AACFRangedWeapon::InitShooting()
{
    if (ShootingComp) {
        ACharacter* charOwner = Cast<ACharacter>(ItemOwner);
        if (charOwner) {
            ShootingComp->SetupShootingComponent(charOwner, Mesh);
        }
    }
}

void AACFRangedWeapon::Shoot_Implementation(const FRotator& deltaRot, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /*= nullptr);*/)
{
    if (ItemOwner && ShootingComp) {
        const FRotator EyesRotation = ItemOwner->GetControlRotation();
        ShootingComp->ShootAtDirection(EyesRotation + deltaRot, charge, projectileOverride);
    }
}

void AACFRangedWeapon::ShootAtDirection_Implementation(const FRotator& direction, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /* = nullptr);*/)
{
    if (ShootingComp) {
        ShootingComp->ShootAtDirection(direction, charge, projectileOverride);
    }
}

void AACFRangedWeapon::SwipeTraceShoot_Implementation(EShootTargetType targetType)
{

    if (ShootingComp && ItemOwner) {

        const FTransform targetTransform = UACFItemSystemFunctionLibrary::GetShootTransform(ItemOwner, targetType);

        const FVector direction = targetTransform.GetUnitAxis(EAxis::X);
        const FVector startTrace = targetTransform.GetLocation();
        //  const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());
        ShootingComp->SwipeTraceShootAtDirection(startTrace, direction);
    }
}

void AACFRangedWeapon::ShootProjectile_Implementation(EShootTargetType targetType /*= EShootTargetType::WeaponTowardsFocus*/, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /*= nullptr*/)
{
    if (ShootingComp && ItemOwner) {

        const FTransform targetTransform = UACFItemSystemFunctionLibrary::GetShootTransform(ItemOwner, targetType);

        const FVector direction = targetTransform.GetUnitAxis(EAxis::X);
        const FVector startTrace = targetTransform.GetLocation();
        //  const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());
        ShootingComp->ShootAtDirection(direction.ToOrientationRotator(), charge, projectileOverride);
    }
}

void AACFRangedWeapon::Reload_Implementation(bool bTryToEquipAmmo)
{
    ShootingComp->Reload(bTryToEquipAmmo);
}

bool AACFRangedWeapon::CanShoot() const
{
    return ShootingComp->CanShoot();
}

bool AACFRangedWeapon::NeedsReload() const
{
    return ShootingComp->NeedsReload();
}

void AACFRangedWeapon::OnItemEquipped_Implementation()
{
    Super::OnItemEquipped_Implementation();
    if (ShootingComp) {
        ShootingComp->SetupShootingComponent(ItemOwner, Mesh);
        ShootingComp->Reload(TryEquipAmmos);
    }
}

void AACFRangedWeapon::ShootAtActor_Implementation(const AActor* target, float randomDeviation /*= 50.f*/, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /*= nullptr);*/)
{
    if (ItemOwner && ShootingComp) {
        ShootingComp->ShootAtActor(target, randomDeviation, charge, projectileOverride);
    }
}
