// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACFRiderComponent.h"
#include "ACFMountPointComponent.h"
#include "ACFMountComponent.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFGroupAIComponent.h"
#include "Components/ACFTeamManagerComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <TimerManager.h>
#include "NavigationSystem/Public/NavigationSystem.h"

// Sets default values for this component's properties
UACFRiderComponent::UACFRiderComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    // ...
}

// Called when the game starts
void UACFRiderComponent::BeginPlay()
{
    Super::BeginPlay();

    charOwner = Cast<ACharacter>(GetOwner());
}

void UACFRiderComponent::StartMount_Implementation(UACFMountableComponent* mount)
{
    if (mount && mount->CanBeMounted() && charOwner) {
        Mount = mount;
        HandlePossession();
        Internal_Ride(true);

    } else {
        UE_LOG(LogTemp, Warning, TEXT("Impossible to Mount!"));
    }
}

bool UACFRiderComponent::StartMount_Validate(UACFMountableComponent* mount)
{
    return true;
}

void UACFRiderComponent::OnRep_IsRiding()
{
    Internal_SetMountCollisionsEnabled(bIsRiding);
    OnRidingStateChanged.Broadcast(bIsRiding);
}

void UACFRiderComponent::StartDismount_Implementation(const FName& dismountPoint /*= NAME_None*/)
{
    if (!bIsRiding) {
        return;
    }
    FinishDismount(dismountPoint);
}

bool UACFRiderComponent::StartDismount_Validate(const FName& dismountPoint /*= NAME_None*/)
{
    return true;
}

void UACFRiderComponent::HandlePossession()
{
    AController* contr = charOwner->GetController();
    APawn* mountedChar = Cast<APawn>(Mount->GetPawnOwner());
    if (contr && Mount->NeedsPossession()) {
        mountedChar->DetachFromControllerPendingDestroy();
        contr->Possess(mountedChar);
        charOwner->SpawnDefaultController();
    }
}

void UACFRiderComponent::Internal_Ride(bool inIsRiding)
{
    bIsRiding = inIsRiding;
    Internal_SetMountCollisionsEnabled(bIsRiding);
    OnRidingStateChanged.Broadcast(bIsRiding);
    if (Mount) {
        if (bIsRiding) {
            Mount->SetRider(charOwner);
            Internal_AttachToMount();
        }
        Mount->SetMounted(bIsRiding);
    }
}

FGameplayTag UACFRiderComponent::GetMountTypeTag() const
{
    if (Mount) {
        return Mount->GetMountTag();
    }
    return FGameplayTag();
}

void UACFRiderComponent::Internal_AttachToMount()
{
    if (!Mount) {
        StartDismount();
        return;
    }
    charOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  //  charOwner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    charOwner->AttachToComponent(Mount->GetMountMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        Mount->GetMountPoint());
    charOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
}

void UACFRiderComponent::Internal_Mount()
{
    Internal_Ride(true);
}

void UACFRiderComponent::Internal_SetMountCollisionsEnabled(const bool bMounted)
{
    if (!charOwner) {
        return;
    }

    if (bMounted) {
        charOwner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        charOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    } else {
        charOwner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        charOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

void UACFRiderComponent::OnRep_Mount()
{
    if (Mount) {
        Internal_AttachToMount();
    } else {
        Internal_DetachFromMount();
    }
}

void UACFRiderComponent::FinishDismount(const FName& dismountPoint /*= NAME_None*/)
{
    if (charOwner && bIsRiding && Mount) {
        Internal_DetachFromMount();
        const UACFMountPointComponent* pointComp = Mount->GetDismountPoint(dismountPoint);
        if (pointComp) {
            const FVector dismLocation = pointComp->GetComponentLocation();
      
            charOwner->SetActorLocation(dismLocation, true);
            const FRotator newRot(0.f, charOwner->GetActorRotation().Yaw, 0.f);
            charOwner->SetActorRotation(newRot);
        }
        APawn* mountChar = Mount->GetMountOwner();
        if (mountChar) {
            AController* contr = mountChar->GetController();
            if (contr && Mount->NeedsPossession()) {
                charOwner->DetachFromControllerPendingDestroy();

                contr->Possess(charOwner);
                mountChar->SpawnDefaultController();
                // QUICK FIX
                FRotator rot = contr->GetControlRotation();
                rot.Roll = 0.f;
                rot.Pitch = 0.f;
                contr->SetControlRotation(rot);
            }
        }
        Internal_Ride(false);
        Mount = nullptr;
    }
}

void UACFRiderComponent::Internal_DetachFromMount()
{
    charOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    charOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

class APawn* UACFRiderComponent::GetMount() const
{
    if (Mount) {
        return Mount->GetMountOwner();
    }
    return nullptr;
}

void UACFRiderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFRiderComponent, bIsRiding);
    DOREPLIFETIME(UACFRiderComponent, Mount);
}
