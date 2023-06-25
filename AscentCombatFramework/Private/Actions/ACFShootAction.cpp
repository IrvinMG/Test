// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Actions/ACFShootAction.h"
#include "Actors/ACFCharacter.h"
#include "Items/ACFRangedWeapon.h"

UACFShootAction::UACFShootAction()
{
    ActionConfig.bPerformableInAir = true;
}

bool UACFShootAction::CanExecuteAction_Implementation(class ACharacter* owner)
{
	AACFCharacter* acfCharacter = Cast<AACFCharacter>(owner);

    if (acfCharacter) {
        const AACFWeapon* mainWeapon = acfCharacter->GetEquipmentComponent()->GetCurrentMainWeapon();
        if (mainWeapon) {
            const UACFShootingComponent* shootingComp = mainWeapon->FindComponentByClass<UACFShootingComponent>();
            if (shootingComp) {
                return shootingComp->CanShoot();
            };
        }
    }
    return false;
}

void UACFShootAction::OnNotablePointReached_Implementation()
{
    Super::OnNotablePointReached_Implementation();

	AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

    if (acfCharacter) {
        const UACFEquipmentComponent* equip = acfCharacter->GetEquipmentComponent();
        AACFWeapon* weap = equip->GetCurrentMainWeapon();
        AACFRangedWeapon* rangedWeapon = Cast<AACFRangedWeapon>(weap);

        if (rangedWeapon) {
            switch (rangedWeapon->GetShootingType()) {
            case EShootingType::ESwipeTrace:
                rangedWeapon->SwipeTraceShoot(SwipeTargetType);
                break;
            case EShootingType::EProjectile:
                rangedWeapon->Shoot(DeltaDirection, 1.f, ProjectileOverride);
                break;
            }
        }
    }
}
