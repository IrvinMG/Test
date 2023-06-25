// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFRagdollComponent.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Game/ACFDamageType.h"
#include <Animation/AnimMontage.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/World.h>
#include <TimerManager.h>

#include "Animation/ACFAnimInstance.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFRagdollMasterComponent.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/GameModeBase.h>
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UACFRagdollComponent::UACFRagdollComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UACFRagdollComponent::BeginPlay()
{
    Super::BeginPlay();

    characterOwner = Cast<AACFCharacter>(GetOwner());

    if (!characterOwner) {
        UE_LOG(LogTemp, Log, TEXT("Ragdoll component must be placed on a ACFCharacter! - UACFRagdollComponent"));
        return;
    }
    UACFDamageHandlerComponent* damageComp = characterOwner->GetDamageHandlerComponent();

    damageComp->OnDamageReceived.AddDynamic(this, &UACFRagdollComponent::HandleDamageReceived);
}

void UACFRagdollComponent::GoRagdoll(const FRagdollImpulse& impulse)
{

    if (!GetMesh() || !GetCapsuleComponent()) {
        return;
    }
    const FVector beforeLoc = GetMesh()->GetRelativeLocation();
    const FQuat beforeRot = GetMesh()->GetRelativeRotation().Quaternion();

    if (!bIsKinematic) {
        beforeRagdollTransform.SetLocation(beforeLoc);
        beforeRagdollTransform.SetRotation(beforeRot);
    }

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    oldConfig = GetMesh()->GetCollisionResponseToChannels();
    GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, true);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    GetMesh()->SetCollisionProfileName("BlockAll");
    const FVector impulseVec = impulse.ImpulseDirection.GetSafeNormal() * impulse.ImpulseIntensity;

    characterOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    GetMesh()->AddImpulse(impulseVec, impulse.BoneName, true);
    SetIsRagdoll(true);
}

void UACFRagdollComponent::GoRagdollForDuration(const FRagdollImpulse& impulse, float ragdollDuration)
{
    GoRagdoll(impulse);
    UWorld* world = GetWorld();
    if (world) {
        FTimerHandle ragdollTimer;
        world->GetTimerManager().SetTimer(
            ragdollTimer, this, &UACFRagdollComponent::RecoverFromRagdoll,
            ragdollDuration, false);
    }
}

void UACFRagdollComponent::GoRagdollFromDamage(const FACFDamageEvent& damageEvent, float RagdollDuration /*= -1.f*/)
{
    FRagdollImpulse impulseRag;

    if (!damageEvent.DamageClass) {
        return;
    }

    const UDamageType* damageObj = damageEvent.DamageClass.GetDefaultObject();
    impulseRag.ImpulseIntensity = damageObj->DamageImpulse;
    impulseRag.BoneName = damageEvent.hitResult.BoneName;
    impulseRag.ImpulseDirection = -damageEvent.hitDirection;

    GoRagdollForDuration(impulseRag, RagdollDuration);
}

void UACFRagdollComponent::RecoverFromRagdoll()
{

    if (!bIsRagdoll) {
        return;
    }

    if (!GetMesh() || !GetCapsuleComponent()) {
        return;
    }
    UpdateOwnerLocation();
    UAnimMontage* montage = GetGetUpMontage();
    if (montage) {
        UACFActionsManagerComponent* actionsComp = characterOwner->GetActionsComponent();
        if (actionsComp) {
            FACFMontageInfo montageInfo = FACFMontageInfo(montage);
            actionsComp->PlayReplicatedMontage(montageInfo);

            UWorld* world = GetWorld();
            if (world) {
                FTimerHandle timer;
                world->GetTimerManager().SetTimer(
                    timer, this, &UACFRagdollComponent::TerminateRagdoll,
                    .5f, false);
            }
        }
    } else {
        TerminateRagdoll();
    }
}

void UACFRagdollComponent::TerminateRagdoll()
{
    SetIsRagdoll(false);

    characterOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToChannels(oldConfig);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, false);

    characterOwner->AssignTeam(characterOwner->GetCombatTeam());
    if (!bIsKinematic) {
        GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        GetMesh()->SetRelativeLocation(beforeRagdollTransform.GetLocation());
        GetMesh()->SetRelativeRotation(beforeRagdollTransform.GetRotation());
    }
}

void UACFRagdollComponent::HandleDamageReceived(const FACFDamageEvent& damageEvent)
{
    if (!bTestRagdollOnHit) {
        return;
    }

    if (!damageEvent.DamageClass) {
        return;
    }

    const UDamageType* damageObj = damageEvent.DamageClass.GetDefaultObject();

    if (damageObj->DamageImpulse >= ImpulseResistance) {
        GoRagdollFromDamage(damageEvent, DefaultRagdollDuration);
    }
}

void UACFRagdollComponent::UpdateOwnerLocation()
{
    if (characterOwner) {
        FVector locDest = GetMesh()->GetSocketLocation(PelvisBone);
        locDest.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
        characterOwner->SetActorLocation(locDest);
        const FRotator localRot = GetMesh()->GetSocketRotation(PelvisBone);
        const bool bFaceUp = IsFaceUp();
        FRotator finalRot = FRotator(0.f, 0.f, 0.f);
        finalRot.Yaw = localRot.Yaw;
        characterOwner->SetActorRotation(finalRot);
    }
}

bool UACFRagdollComponent::IsFaceUp()
{
    return (GetMesh()->GetSocketRotation(PelvisBone).Yaw < 0.f);
}

void UACFRagdollComponent::SetIsRagdoll(bool inIsRagdoll)
{
    bIsRagdoll = inIsRagdoll;

    AGameModeBase* gameMode = UGameplayStatics::GetGameMode(this);
    if (gameMode) {
        UACFRagdollMasterComponent* ragdollMaster = gameMode->FindComponentByClass<UACFRagdollMasterComponent>();
        if (ragdollMaster) {
            if (bIsRagdoll) {
                ragdollMaster->AddComponent(this);
            } else {
                ragdollMaster->RemoveComponent(this);
            }
        } else {
            UE_LOG(LogTemp, Error, TEXT("Add Ragdoll Master to your Game Mode!"));
        }
    }
    OnRagdollStateChanged.Broadcast(bIsRagdoll);
}

UAnimMontage* UACFRagdollComponent::GetGetUpMontage()
{
    if (IsFaceUp()) {
        return GetUpFront;
    }
    return GetUpBack;
}
