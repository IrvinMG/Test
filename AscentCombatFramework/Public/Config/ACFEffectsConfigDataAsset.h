// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved.

#pragma once

#include "ACMTypes.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFTypes.h"

#include "ACFEffectsConfigDataAsset.generated.h"

/**
 *
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFEffectsConfigDataAsset : public UPrimaryDataAsset {
    GENERATED_BODY()

public:
    bool ShouldCheckForSurface() const
    {
        return bCheckForSurface;
    }

    void GetDefaultFootstepFX(FBaseFX& outFX) const
    {
        outFX = DefaultFootstepFX;
    }

    bool TryGetFootstepFXBySurfaceType(const EPhysicalSurface& inSurface, FBaseFX& outFX) const
    {
        if (FootstepEffectsBySurface.Contains(inSurface)) {
            outFX = *FootstepEffectsBySurface.Find(inSurface);
            return true;
        }
        outFX = DefaultFootstepFX;
        return false;
    }

    bool TryGetDamageEffectsByHitReactionAndDamageType(const FGameplayTag& HitReaction,
        const TSubclassOf<UDamageType>& damageType, FBaseFX& outFX)
    {
        FEffectByDamageType* effectList;
        if (HitReaction == FGameplayTag()) {
            effectList = DamageEffectsByHitReaction.FindByKey(UACFFunctionLibrary::GetDefaultHitState());
        } else {
            effectList = DamageEffectsByHitReaction.FindByKey(HitReaction);
        }

        if (!effectList) {
            return false;
        }

        if (effectList->EffectByDamageType.Contains(damageType)) {
            outFX = *effectList->EffectByDamageType.Find(damageType);
            return true;
        } else {
            for (auto& currentDam : effectList->EffectByDamageType) {
                if (damageType->IsChildOf(currentDam.Key)) {
                    outFX = currentDam.Value;
                    return true;
                }
            }
        }

        return false;
    }

protected:
    /*Enable terrain material check for footsteps*/
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF| Footstep")
    bool bCheckForSurface = false;

    /*Effect played on Footstep when CheckForSurface is DISABLED*/
    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "!bCheckForSurface"), EditDefaultsOnly, Category = "ACF| Footstep")
    FBaseFX DefaultFootstepFX;

    /*Effects played on Footstep when CheckForSurface is ENABLED, depending on the actual
    terrain surface physical material*/
    UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "bCheckForSurface"), EditDefaultsOnly, Category = "ACF| Footstep")
    TMap<TEnumAsByte<EPhysicalSurface>, FBaseFX> FootstepEffectsBySurface;

    /*Effects triggered when this character gets hit depending on the DamageType Received.
    Location will always be the impact point*/
    UPROPERTY(BlueprintReadOnly, meta = (TitleProperty = "TagName"), EditDefaultsOnly, Category = "ACF | Damages")
    TArray<FEffectByDamageType> DamageEffectsByHitReaction;
};
