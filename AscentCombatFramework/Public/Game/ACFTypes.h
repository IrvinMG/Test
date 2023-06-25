// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFActionTypes.h"
#include "ACMTypes.h"
#include "ARSTypes.h"
#include "Camera/CameraShakeBase.h"
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/ACFEntityInterface.h"
#include "NiagaraSystem.h"
#include "Templates/SubclassOf.h"
#include <Engine/DataTable.h>
#include <GameFramework/DamageType.h>
#include <GameplayTagContainer.h>

#include "ACFTypes.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EDeathType : uint8 {
    EGoRagdoll = 0 UMETA(DisplayName = "Go Ragdoll"),
    EDeathAction UMETA(DisplayName = "Trigger Death Action"),
    EDoNothing UMETA(DisplayName = "Do Nothing"),
};

USTRUCT(BlueprintType)
struct FDamageInfluence {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FGameplayTag Parameter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float ScalingFactor = 1.f;
};

USTRUCT(BlueprintType)
struct FDamageInfluences {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<FDamageInfluence> AttackParametersInfluence;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<FDamageInfluence> DefenseParametersPercentages;
};

UENUM(BlueprintType)
enum class EDamageActivationType : uint8 {
    ERight UMETA(DisplayName = "Right Weapon Only"),
    ELeft UMETA(DisplayName = "Left Weapon Only"),
    EBoth UMETA(DisplayName = "Left And Right Weapon"),
    EPhysical UMETA(DisplayName = "Physical Damage"),
};

UENUM(BlueprintType)
enum class EBattleState : uint8 {
    EExploration UMETA(DisplayName = "Exploration"),
    EBattle UMETA(DisplayName = "Battle"),
};

UENUM(BlueprintType)
enum class EDamageZone : uint8 {
    ENormal UMETA(DisplayName = "Normal Zone"),
    EImmune UMETA(DisplayName = "Zero Damage Zone"),
    EHighDefense UMETA(DisplayName = "High Defense Zone"),
    EHighDamage UMETA(DisplayName = "High Damage Zone"),
};

USTRUCT(BlueprintType)
struct FRagdollImpulse {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    float ImpulseIntensity = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FVector ImpulseDirection = FVector(0.f, 0.f, -1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FName BoneName = "pelvis";

    FRagdollImpulse() {};
};

UENUM(BlueprintType)
enum class EHitDirection : uint8 {
    EFront = 0,
    EFrontLeft = 1,
    EFrontRight = 2,
    ELeftLeg = 3,
    ERightLeg = 4,
    EHead = 5,
    ERight = 6,
    ELeft = 7,
    EBack = 8
};

UENUM(BlueprintType)
enum class EActionDirection : uint8 {
    Front = 0 UMETA(DisplayName = "Front Only"),
    Back = 1 UMETA(DisplayName = "Back Only"),
    Left = 2 UMETA(DisplayName = "Left Only"),
    Right = 3 UMETA(DisplayName = "Right Only"),
    EveryDirection = 4 UMETA(DisplayName = "Every Direction"),
};

USTRUCT(BlueprintType)
struct FImpactEffect {
    GENERATED_BODY()

public:

    FImpactEffect()
    {
        bShouldShakeCamera= false;
        CameraShake = nullptr;
        bShouldMakeDamage = false;
        DamageTraceDuration = .2f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    bool bShouldShakeCamera;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
    TSubclassOf<class UCameraShakeBase> CameraShake;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
    float ShakeFalloff = 100.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
    float ShakeRadius = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    bool bShouldMakeDamage;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldMakeDamage), BlueprintReadWrite, Category = ACF)
    FName DamageTraceName;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldMakeDamage), BlueprintReadWrite, Category = ACF)
    float DamageTraceDuration;
};

USTRUCT(BlueprintType)
struct FActionChances : public FTableRowBase {
    GENERATED_BODY()

public:

    FActionChances() {
        AllowedFromDirection = EActionDirection::Front;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    EActionDirection AllowedFromDirection;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float ChancePercentage = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float BTWaitTime = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    EActionPriority Priority = EActionPriority::ELow;
};

USTRUCT(BlueprintType)
struct FActionsChances {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<FActionChances> PossibleActions;
};

USTRUCT(BlueprintType)
struct FOnHitActionChances : public FActionChances {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<TSubclassOf<UDamageType>> AllowedDamageTypes;
};

USTRUCT(BlueprintType)
struct FEffectByDamageType : public FACFStruct {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TMap<TSubclassOf<UDamageType>, FBaseFX> EffectByDamageType;
};

UENUM(BlueprintType)
enum class ECombatType : uint8 {
    EUnarmed UMETA(DisplayName = "Unarmed"),
    EMelee UMETA(DisplayName = "Melee"),
    ERanged UMETA(DisplayName = "Ranged"),
};

UENUM(BlueprintType)
enum class ECombatBehaviorType : uint8 {
    EMelee UMETA(DisplayName = "Melee"),
    ERanged UMETA(DisplayName = "Ranged"),
};

UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFTypes : public UObject {
    GENERATED_BODY()
};
