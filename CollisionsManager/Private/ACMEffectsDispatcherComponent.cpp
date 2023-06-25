// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACMEffectsDispatcherComponent.h"
#include "ACMImpactsFXDataAsset.h"
#include "ACMTypes.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundBase.h>
#include <Sound/SoundCue.h>
#include "Particles/ParticleSystem.h"
#include "NiagaraCommon.h"
#include "Niagara/Classes/NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values for this component's properties
UACMEffectsDispatcherComponent::UACMEffectsDispatcherComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    // ...
}

// Called when the game starts
void UACMEffectsDispatcherComponent::BeginPlay()
{
    Super::BeginPlay();
    // ...
}

void UACMEffectsDispatcherComponent::ClientsPlayEffect_Implementation(const FActionEffect& effect, class ACharacter* instigator)
{
    Internal_PlayEffect(instigator, effect);
}

void UACMEffectsDispatcherComponent::ClientsPlayReplicatedEffect_Implementation(const FImpactFX& FXtoPlay)
{
    SpawnSoundAndParticleAtLocation(FXtoPlay);
}

void UACMEffectsDispatcherComponent::PlayReplicatedActionEffect_Implementation(const FActionEffect& effect, class ACharacter* instigator)
{
    if (instigator) {
        instigator->MakeNoise(effect.NoiseEmitted, instigator, instigator->GetActorLocation());
        ClientsPlayEffect(effect, instigator);
    }
}

void UACMEffectsDispatcherComponent::PlayActionEffectLocally_Implementation(const FActionEffect& effect, class ACharacter* instigator)
{
    Internal_PlayEffect(instigator, effect);
}

void UACMEffectsDispatcherComponent::PlayReplicatedImpact(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, const FVector& impactLocation)
{
    FBaseFX outFX;
    if (TryGetImpactFX(damageImpacting, materialImpacted, outFX)) {
        FImpactFX newImpact = FImpactFX(outFX, impactLocation);
        PlayReplicatedEffect(newImpact);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Missing FXs for impacts!! - UACMEffectsDispatcherComponent "));
    }
}

void UACMEffectsDispatcherComponent::PlayReplicatedEffect_Implementation(const FImpactFX& FXtoPlay)
{
    ClientsPlayReplicatedEffect(FXtoPlay);
}

void UACMEffectsDispatcherComponent::PlayEffectLocally(const FImpactFX& effect)
{
    SpawnSoundAndParticleAtLocation(effect);
}

bool UACMEffectsDispatcherComponent::TryGetImpactFX(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, FBaseFX& outFXtoPlay)
{
    if (ImpactFXs) {
        return ImpactFXs->TryGetImpactFX(damageImpacting, materialImpacted, outFXtoPlay);
    }
    return false;
}

void UACMEffectsDispatcherComponent::Internal_PlayEffect(class ACharacter* instigator, const FActionEffect& effect)
{
    if (instigator) {
        switch (effect.SpawnLocation) {
        case ESpawnFXLocation::ESpawnOnActorLocation:
        case ESpawnFXLocation::ESpawnAttachedToSocketOrBone:
            SpawnSoundAndParticleAttached(effect, instigator);
            break;
        case ESpawnFXLocation::ESpawnAtLocation:
            SpawnSoundAndParticleAtLocation(FImpactFX(effect, effect.RelativeOffset));
            break;
        default:
            SpawnSoundAndParticleAttached(effect, instigator);
            break;
        }
    }
}

void UACMEffectsDispatcherComponent::SpawnSoundAndParticleAttached(const FActionEffect& effect, ACharacter* instigator)
{
    if (effect.ActionParticle) {
        UGameplayStatics::SpawnEmitterAttached(effect.ActionParticle, instigator->GetMesh(), effect.SocketOrBoneName,
            effect.RelativeOffset.GetLocation(), effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D());
    }
    
    if (effect.ActionSound) {
        UGameplayStatics::SpawnSoundAttached(effect.ActionSound, instigator->GetMesh(), effect.SocketOrBoneName);
    }

    if (effect.NiagaraParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAttached(effect.NiagaraParticle, instigator->GetMesh(), effect.SocketOrBoneName,
            effect.RelativeOffset.GetLocation(), effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D(),
            EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease);
    }
}

void UACMEffectsDispatcherComponent::SpawnSoundAndParticleAtLocation(const FImpactFX& effect)
{
    if (effect.ActionParticle) {
        UGameplayStatics::SpawnEmitterAtLocation(this, effect.ActionParticle, effect.SpawnLocation.GetLocation(),
            effect.SpawnLocation.GetRotation().Rotator(), effect.SpawnLocation.GetScale3D());
    }

    if (effect.ActionSound) {
        UGameplayStatics::SpawnSoundAtLocation(this, effect.ActionSound, effect.SpawnLocation.GetLocation());
    }

    if (effect.NiagaraParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, effect.NiagaraParticle, effect.SpawnLocation.GetLocation(),
            effect.SpawnLocation.GetRotation().Rotator(), effect.SpawnLocation.GetScale3D());
    }
}
