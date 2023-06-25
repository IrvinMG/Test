// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Actions/ACFShootAtScreenCenterAction.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFShootingComponent.h"
#include "GameFramework/PlayerController.h"
#include "Items/ACFWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

bool UACFShootAtScreenCenterAction::CanExecuteAction_Implementation(class ACharacter* owner)
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

void UACFShootAtScreenCenterAction::OnNotablePointReached_Implementation()
{
    Super::OnNotablePointReached_Implementation();
    ShootAtScreenCenter();

}

UACFShootAtScreenCenterAction::UACFShootAtScreenCenterAction()
{
    ScreenPosition.X = 0.5f;
    //slightly higher for gravity
    ScreenPosition.Y = 0.45f;
}

void UACFShootAtScreenCenterAction::ShootAtScreenCenter(const float charge)
{
	AACFCharacter* acfCharacter = Cast<AACFCharacter>(CharacterOwner);

	if (acfCharacter) {
		const AACFWeapon* mainWeapon = acfCharacter->GetEquipmentComponent()->GetCurrentMainWeapon();
		if (mainWeapon) {
			UACFShootingComponent* shootingComp = mainWeapon->FindComponentByClass<UACFShootingComponent>();
			const APlayerController* playerCont = UGameplayStatics::GetPlayerController(CharacterOwner, 0);

			if (shootingComp && playerCont) {

				const FVector startPos = shootingComp->GetShootingSocketPosition();
				int32 hor, ver;
				playerCont->GetViewportSize(hor, ver);
				FVector position, direction;
				playerCont->DeprojectScreenPositionToWorld(hor * ScreenPosition.X, ver * ScreenPosition.Y, position, direction);
				const FVector endPos = startPos + (direction * ShootTraceLength);

				TArray<AActor*> actorsToIgnore;
				actorsToIgnore.Add(CharacterOwner);

				FRotator finalDir;
				FHitResult outResult;
				if (UKismetSystemLibrary::LineTraceSingle(CharacterOwner, startPos, endPos, TraceChannel,
					true, actorsToIgnore, Debug, outResult, true)) {
					finalDir = UKismetMathLibrary::FindLookAtRotation(startPos, outResult.Location);
				}
				else {
					finalDir = UKismetMathLibrary::FindLookAtRotation(startPos, endPos);
				}
				shootingComp->ShootAtDirection(finalDir, charge, ProjectileOverride);
			};
		}
	}
}
