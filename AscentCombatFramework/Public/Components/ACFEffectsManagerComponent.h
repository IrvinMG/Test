// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Game/ACFDamageType.h"
#include <Engine/EngineTypes.h>

#include "ACFEffectsManagerComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFEffectsManagerComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFEffectsManagerComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class ACharacter* CharacterOwner;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
    class UACFEffectsConfigDataAsset* CharacterEffectsConfig;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF| Footstep")
    float TraceLengthByActorLocation = 200.f;

    /*The noise emitted while moving by this character. This noise is used as a check
    for AI Perceptions*/
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF| Footstep")
    TMap<ELocomotionState, float> FootstepNoiseByLocomotionState;

    /*The noise emitted while moving by this character. This noise is used as a check
    for AI Perceptions*/
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF| Footstep")
    TMap<ELocomotionState, float> FootstepNoiseByLocomotionStateWhenCrouched;

public:
    UFUNCTION(BlueprintCallable, Category = ACF)
    void TriggerFootstepFX(FName footBone = NAME_None);

    UFUNCTION(BlueprintCallable, Category = ACF)
    EPhysicalSurface GetCurrentTerrain();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnDamageImpactReceived(const FACFDamageEvent& damageEvent);
    virtual void OnDamageImpactReceived_Implementation(const FACFDamageEvent& damageEvent);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void PlayHitReactionEffect(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType, const FTransform& hitLocation);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool TryGetDamageFX(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType, FBaseFX& outFX);

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetNoiseToEmitForCurrentLocomotionState() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetNoiseToEmitByLocomotionState(ELocomotionState locState) const;

private:
    UFUNCTION()
    void HandleDamageReceived(const FACFDamageEvent& damageEvent);

    void PlayCurrentActionEffect(const FActionEffect& effect);
};
