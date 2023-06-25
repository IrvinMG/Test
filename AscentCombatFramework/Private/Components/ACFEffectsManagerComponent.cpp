// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFEffectsManagerComponent.h"
#include "ACMCollisionsFunctionLibrary.h"
#include "ACMTypes.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Config/ACFEffectsConfigDataAsset.h"
#include "Game/ACFFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include <CollisionQueryParams.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "Engine/World.h"

// Sets default values for this component's properties
UACFEffectsManagerComponent::UACFEffectsManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UACFEffectsManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    CharacterOwner = Cast<ACharacter>(GetOwner());

    UACFDamageHandlerComponent* DamageHanlder = GetOwner()->FindComponentByClass<UACFDamageHandlerComponent>();

    if (DamageHanlder) {
        DamageHanlder->OnDamageReceived.AddDynamic(this, &UACFEffectsManagerComponent::HandleDamageReceived);
    }

    FootstepNoiseByLocomotionState.Add(ELocomotionState::EWalk, .2f);
    FootstepNoiseByLocomotionState.Add(ELocomotionState::EJog, .6f);
    FootstepNoiseByLocomotionState.Add(ELocomotionState::ESprint, 1.f);

    FootstepNoiseByLocomotionStateWhenCrouched.Add(ELocomotionState::EWalk, .1f);
    FootstepNoiseByLocomotionStateWhenCrouched.Add(ELocomotionState::EJog, .3f);
    FootstepNoiseByLocomotionStateWhenCrouched.Add(ELocomotionState::EJog, .5f);
}

void UACFEffectsManagerComponent::TriggerFootstepFX(FName footBone /*= NAME_None*/)
{
    if (!CharacterEffectsConfig) {
        UE_LOG(LogTemp, Error, TEXT("Missing Characters Effect Config! - UACFEffectsManagerComponent::TriggerFootstepFX "));
        return;
    }

    FBaseFX baseFX;

    if (CharacterEffectsConfig->ShouldCheckForSurface()) {
        CharacterEffectsConfig->TryGetFootstepFXBySurfaceType(GetCurrentTerrain(), baseFX);
    } else {
        CharacterEffectsConfig->GetDefaultFootstepFX(baseFX);
    }

    FVector ownerLocation;

    if (footBone != NAME_None) {
        ownerLocation = CharacterOwner->GetMesh()->GetSocketLocation(footBone);
    } else {
        ownerLocation = CharacterOwner->GetActorLocation();
    }

    FImpactFX fxToPlay = FImpactFX(baseFX, ownerLocation);

    const float noise = GetNoiseToEmitForCurrentLocomotionState();

    if (UKismetSystemLibrary::IsServer(this) && noise != 0.f) {
        CharacterOwner->MakeNoise(noise, CharacterOwner, CharacterOwner->GetActorLocation());
    }

    UACMCollisionsFunctionLibrary::PlayEffectLocally(fxToPlay, this);
}

EPhysicalSurface UACFEffectsManagerComponent::GetCurrentTerrain()
{
    ensure(CharacterOwner);
    if (CharacterOwner) {
        FCollisionQueryParams outTraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, CharacterOwner);

        outTraceParams.bTraceComplex = true;
        outTraceParams.bReturnPhysicalMaterial = true;

        const FVector Start = CharacterOwner->GetActorLocation();
        const FVector End = Start + (FVector(0, 0, 1) * TraceLengthByActorLocation * -1);

        // Re-initialize hit info
        FHitResult outTerrain;

        const bool bHit = GetWorld()->LineTraceSingleByObjectType(outTerrain, Start,End,  ECC_WorldStatic, outTraceParams);

        if (bHit) {
            const UPhysicalMaterial* PhysicsMtl = outTerrain.PhysMaterial.Get();
            const EPhysicalSurface surface = PhysicsMtl->SurfaceType;
            return surface;
        }
    }

    return EPhysicalSurface::SurfaceType_Max;
}

void UACFEffectsManagerComponent::HandleDamageReceived(const FACFDamageEvent& damageEvent)
{
    PlayHitReactionEffect(damageEvent.HitResponseAction, damageEvent.DamageClass, FTransform(damageEvent.hitResult.ImpactPoint));
    OnDamageImpactReceived(damageEvent);
}

void UACFEffectsManagerComponent::OnDamageImpactReceived_Implementation(const FACFDamageEvent& damageEvent)
{
}

void UACFEffectsManagerComponent::PlayCurrentActionEffect(const FActionEffect& effect)
{
    FActionEffect outEffect = effect;
    const UACFDamageHandlerComponent* damageComp = GetOwner()->FindComponentByClass<UACFDamageHandlerComponent>();
    if (damageComp) {
        const FVector finalPos = effect.RelativeOffset.GetLocation() + damageComp->GetLastDamageInfo().hitResult.Location;
        outEffect.RelativeOffset.SetLocation(finalPos);

        UACFFunctionLibrary::PlayActionEffect(outEffect, CharacterOwner, this);
    } else {
        UE_LOG(LogTemp, Error, TEXT("No Damage Handler comp in your character!"));
    }
}

void UACFEffectsManagerComponent::PlayHitReactionEffect(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType,
    const FTransform& hitLocation)
{
    FImpactFX outFX;
    if (TryGetDamageFX(HitRection, DamageType, outFX)) {
        outFX.SpawnLocation = hitLocation;
        UACMCollisionsFunctionLibrary::PlayReplicatedEffect(outFX, this);
    } else {
        UE_LOG(LogTemp, Error, TEXT("Missing Characters Effect Config! - UACFEffectsManagerComponent::TriggerFootstepFX "));
    }
}

bool UACFEffectsManagerComponent::TryGetDamageFX(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType, FBaseFX& outFX)
{
    if (CharacterEffectsConfig && CharacterEffectsConfig->TryGetDamageEffectsByHitReactionAndDamageType(HitRection, DamageType, outFX)) {
        return true;
    }
    UE_LOG(LogTemp, Error, TEXT("Missing Characters Effect Config! - UACFEffectsManagerComponent::TriggerFootstepFX "));
    return false;
}

float UACFEffectsManagerComponent::GetNoiseToEmitForCurrentLocomotionState() const
{
    const UACFCharacterMovementComponent* locComp = CharacterOwner->FindComponentByClass<UACFCharacterMovementComponent>();
    if (locComp) {
        const ELocomotionState currentLocState = locComp->GetCurrentLocomotionState();
        return GetNoiseToEmitByLocomotionState(currentLocState);
    }
    UE_LOG(LogTemp, Error, TEXT("Missing Locomotion Component! - UACFEffectsManagerComponent::GetNoiseToEmitForCurrentLocomotionState "));
    return 0.f;
}

float UACFEffectsManagerComponent::GetNoiseToEmitByLocomotionState(ELocomotionState locState) const
{
    float outNoise = 0.f;
    if (CharacterOwner->bIsCrouched && FootstepNoiseByLocomotionStateWhenCrouched.Contains(locState)) {
        outNoise = *FootstepNoiseByLocomotionStateWhenCrouched.Find(locState);
    } else if (FootstepNoiseByLocomotionState.Contains(locState)) {
        outNoise = *FootstepNoiseByLocomotionState.Find(locState);
    }
    return outNoise;
}
