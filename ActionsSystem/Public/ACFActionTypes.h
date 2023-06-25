// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACMTypes.h"
#include "ARSTypes.h"
#include "Camera/CameraShakeBase.h"
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/ACFEntityInterface.h"
#include "NiagaraSystem.h"
#include "RootMotionModifier.h"
#include "Templates/SubclassOf.h"
#include <Engine/DataTable.h>
#include <GameFramework/DamageType.h>
#include <GameplayTagContainer.h>

#include "ACFActionTypes.generated.h"

class UACFBaseAction;

/**
 *
 */
UENUM(BlueprintType)
enum class EMontageReproductionType : uint8 {
    ERootMotion UMETA(DisplayName = "Root Motion"),
    ERootMotionScaled UMETA(DisplayName = "Root Motion Scaled"),
    EMotionWarped UMETA(DisplayName = "Root Motion Warped"),
    ECurveOverrideSpeed UMETA(DisplayName = "Override Speed with Curve_DEPRECATED"),
    ECurveSnapsOnTarget UMETA(DisplayName = "Override Speed and Snaps To Target_DEPRECATED"),
    ECurveOverrideSpeedAndDirection UMETA(DisplayName = "Override Speed and Direction Vector_DEPRECATED"),
};

UENUM(BlueprintType)
enum class EWarpTargetType : uint8 {
    ETargetTransform UMETA(DisplayName = "Target Transform"),
    ETargetComponent UMETA(DisplayName = "Target Component"),
};

USTRUCT(BlueprintType)
struct FACFWarpInfo {

    GENERATED_USTRUCT_BODY()
public:
    FACFWarpInfo()
    {
        WarpStartTime = 0.f;
        bIgnoreZAxis = true;
        WarpEndTime = 1.f;
        WarpRotationTime = 1.f;
        RotationType = EMotionWarpRotationType::Default;
        SyncPoint = "Target";
        bAutoWarp = true;
        TargetType = EWarpTargetType::ETargetTransform;
    }

    /*Turn this off if you want to use the anim notify instead*/
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    bool bAutoWarp;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    FName SyncPoint;

    UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "bAutoWarp == true"), EditDefaultsOnly, Category = ACF)
    float WarpStartTime;

    UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "bAutoWarp == true"), EditDefaultsOnly, Category = ACF)
    float WarpEndTime;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    float WarpRotationTime;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    bool bIgnoreZAxis;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    EMotionWarpRotationType RotationType;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
    EWarpTargetType TargetType = EWarpTargetType::ETargetTransform;

    UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "TargetType == EWarpTargetType::ETargetComponent"), EditDefaultsOnly, Category = ACF)
    bool bMagneticFollow = true;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    bool bShowWarpDebug = false;
};

USTRUCT(BlueprintType)
struct FACFWarpReproductionInfo {

    GENERATED_USTRUCT_BODY()
public:

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    FACFWarpInfo WarpConfig;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    FVector WarpLocation;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    FRotator WarpRotation;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    USceneComponent* TargetComponent;

    FACFWarpReproductionInfo()
    {
        TargetComponent = nullptr;
        WarpConfig = FACFWarpInfo();
        WarpLocation = FVector(0.f);
        WarpRotation = FRotator(0.f);
    }
};

USTRUCT(BlueprintType)
struct FACFMontageInfo {
    GENERATED_USTRUCT_BODY()
public:
    FACFMontageInfo(class UAnimMontage* inAnimMontage)
    {
        MontageAction = inAnimMontage;
        ReproductionSpeed = 1.f;
        StartSectionName = NAME_None;
        RootMotionScale = 1.f;
        ReproductionType = EMontageReproductionType::ERootMotion;
    }

    FACFMontageInfo()
    {
        MontageAction = nullptr;
        ReproductionSpeed = 1.f;
        StartSectionName = NAME_None;
        RootMotionScale = 1.f;
        ReproductionType = EMontageReproductionType::ERootMotion;
    }

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    class UAnimMontage* MontageAction;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    float ReproductionSpeed;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    FName StartSectionName;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    EMontageReproductionType ReproductionType;

    UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "MontageReproductionType == EMontageReproductionType::ERootMotionScaled"), Category = ACF)
    float RootMotionScale;

    UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "MontageReproductionType == EMontageReproductionType::EMotionWarped"), Category = ACF)
    FACFWarpReproductionInfo WarpInfo;
};

USTRUCT(BlueprintType)
struct FActionState : public FACFStruct {
    GENERATED_BODY()

public:
    FActionState()
    {
        MontageAction = nullptr;
        Action = nullptr;
    }
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    TObjectPtr<class UAnimMontage> MontageAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = ACF)
    TObjectPtr<class UACFBaseAction> Action;
};

USTRUCT(BlueprintType)
struct FActionsSet : public FACFStruct {

    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TSubclassOf<class UACFActionsSet> ActionsSet;
};

USTRUCT(BlueprintType)
struct FBoneSections {

    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FName SectionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    TArray<FName> BoneNames;
};

USTRUCT(BlueprintType)
struct FActionsArray : public FACFStruct {

    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadOnly, Category = ACF)
    TArray<FActionState> Actions;
};

UENUM(BlueprintType)
enum class EActionPriority : uint8 {
    ENone UMETA(DisplayName = "Very Low"),
    ELow UMETA(DisplayName = "Low"),
    EMedium UMETA(DisplayName = "Medium"),
    EHigh UMETA(DisplayName = "High"),
    EHighest UMETA(DisplayName = "Highest"),
};

USTRUCT(BlueprintType)
struct FActionConfig {
    GENERATED_BODY()

public:
    FActionConfig()
    {
        CoolDownTime = 0.f;
    }
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
    TArray<FStatisticValue> ActionCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
    TArray<FAttribute> Requirements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
    int32 RequiredLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
    FAttributesSetModifier AttributeModifier;

    // Decide if the montage of the action is played automatically or you'll need to
    // call ExecuteAction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
    bool bAutoExecute = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
    EMontageReproductionType MontageReproductionType = EMontageReproductionType::ERootMotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MontageReproductionType == EMontageReproductionType::ERootMotionScaled"), Category = ACFAnimConfig)
    float RootMotionScale = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MontageReproductionType == EMontageReproductionType::EMotionWarped"), Category = ACFAnimConfig)
    FACFWarpInfo WarpInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
    bool bPlayRandomMontageSection = false;

    /*Decide if we have to stop AIController's BT during this action*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
    bool bStopBehavioralThree = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
    bool bPlayEffectOnActionStart = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
    FActionEffect ActionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
    bool bPerformableInAir = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
    float CoolDownTime;
};

UCLASS()
class ACTIONSSYSTEM_API UACFAttackTypes : public UObject {
    GENERATED_BODY()
};
