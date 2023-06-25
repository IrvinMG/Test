// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Curves/CurveFloat.h>
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>

#include "ARSTypes.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EModifierType : uint8 {
    EAdditive = 0 UMETA(DisplayName = "Additive"),
    EPercentage UMETA(DisplayName = "Percentage"),
};

UENUM(BlueprintType)
enum class EStatsLoadMethod : uint8 {
    EUseDefaultsWithoutGeneration = 0 UMETA(DisplayName = "Use Defaults Without Generation"),
    EGenerateFromDefaultsPrimary UMETA(DisplayName = "Generate From Default Attributes"),
    ELoadByLevel UMETA(DisplayName = "Load By Actual Level From Curves")
};

UENUM(BlueprintType)
enum class ELevelingType : uint8 {
    ECantLevelUp = 0 UMETA(DisplayName = "Do not use Leveling System"),
    EGenerateNewStatsFromCurves UMETA(DisplayName = "Generate Stats From Curves"),
    EAssignPerksManually UMETA(DisplayName = "Assign Perks Manually"),
};

UENUM(BlueprintType)
enum class EStatisticsType : uint8 {
    EStatistic = 0 UMETA(DisplayName = "Statistic"),
    EPrimaryAttribute UMETA(DisplayName = "Attributes"),
    ESecondaryAttribute UMETA(DisplayName = "Parameters"),
};

USTRUCT(BlueprintType)
struct FBaseModifier {
    GENERATED_BODY()

public:
    // default constructor
    FBaseModifier() { ModType = EModifierType::EAdditive; }

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = ARS)
    FGameplayTag AttributeType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    EModifierType ModType;

    FORCEINLINE bool operator==(const FBaseModifier& Other) const { return this->AttributeType == Other.AttributeType; }

    FORCEINLINE bool operator!=(const FBaseModifier& Other) const { return this->AttributeType != Other.AttributeType; }

    ~FBaseModifier() {};
};

USTRUCT(BlueprintType)
struct FAttributeModifier : public FBaseModifier {
    GENERATED_BODY()

public:
    // default constructor
    FAttributeModifier() {
        AttributeType = FGameplayTag();
        ModType = EModifierType::EAdditive;
        Value = 0.f;
    };

    FAttributeModifier(const FGameplayTag& attTag, const EModifierType inModType, const float inValue) {
        AttributeType = attTag;
        ModType = inModType;
        Value = inValue;
    };


    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = ARS)
    float Value;

    ~FAttributeModifier() {};
};

USTRUCT(BlueprintType)
struct FStatisticsModifier : public FBaseModifier {
    GENERATED_BODY()

public:
    // default constructor
    FStatisticsModifier() {
        AttributeType = FGameplayTag();
        ModType = EModifierType::EAdditive;
        MaxValue = 0.f;
        RegenValue = 0.f;
    };

    FStatisticsModifier(const FGameplayTag& attTag, const EModifierType inModType, const float inMaxValue, const float inRegenValue)
    {
        AttributeType = attTag;
        ModType = inModType;
        MaxValue = inMaxValue;
        RegenValue = inRegenValue;
    };

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = ARS)
    float MaxValue;

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = ARS)
    float RegenValue;

    ~FStatisticsModifier() {};
};
USTRUCT(BlueprintType)
struct FStatistic {
    GENERATED_BODY()

public:
    // default constructor
    FStatistic()
    {
        MaxValue = 100.f;
        CurrentValue = bStartFromZero ? 0.f : MaxValue;
        RegenValue = 0.f;
        HasRegeneration = false;
        RegenDelay = 0.0f;
    }
    FStatistic(const FGameplayTag& statTag, float inMaxValue, float inRegenValue, float inRegenDelay = 0.f, bool inStartFromZero = false)
    {
        StatType = statTag;
        MaxValue = inMaxValue;
        CurrentValue = inStartFromZero ? 0.f : MaxValue;
        RegenValue = inRegenValue;
        RegenDelay = inRegenDelay;
        bStartFromZero = inStartFromZero;
    }
    ~FStatistic() {};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    FGameplayTag StatType;

    // maximum stat value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    float MaxValue;

    // current stat value
    UPROPERTY(BlueprintReadOnly, Category = ARS, SaveGame, meta = (ClampMin = 0))
    float CurrentValue;

    /*Indicates if this stat can regenerate over time*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    bool HasRegeneration = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    bool bStartFromZero = false;

    // value added to stat with every timer tick if HasRegeneration is enabled
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    float RegenValue;

    // time you have to wait after the statistic is modified to let regeneration start
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = ARS)
    float RegenDelay;

    FORCEINLINE bool operator==(const FStatistic& Other) const { return this->StatType == Other.StatType; }

    FORCEINLINE bool operator!=(const FStatistic& Other) const { return this->StatType != Other.StatType; }

    FORCEINLINE bool operator==(const FStatisticsModifier& Other) const { return this->StatType == Other.AttributeType; }

    FORCEINLINE bool operator!=(const FStatisticsModifier& Other) const { return this->StatType != Other.AttributeType; }

    FORCEINLINE FStatistic operator+(const FStatistic& Other) const
    {
        FStatistic baseAtt = *(this);
        const float oldmaxvalue = baseAtt.MaxValue;
        if (Other.StatType == this->StatType) {
            baseAtt.MaxValue += Other.MaxValue;
            baseAtt.RegenValue += Other.RegenValue;
            baseAtt.HasRegeneration = baseAtt.RegenValue != 0.f;
            baseAtt.RegenDelay += Other.RegenDelay;
            //   baseAtt.CurrentValue = UARSFunctionLibrary::GetNewCurrentValueForNewMaxValue(CurrentValue, _oldmaxvalue, baseAtt.MaxValue);
        }
        return baseAtt;
    }

    FORCEINLINE FStatistic operator-(const FStatistic& Other) const
    {
        FStatistic baseAtt = *(this);
        const float oldmaxvalue = baseAtt.MaxValue;
        if (Other.StatType == this->StatType) {
            baseAtt.MaxValue -= Other.MaxValue;
            baseAtt.RegenValue -= Other.RegenValue;
            baseAtt.HasRegeneration = baseAtt.RegenValue != 0.f;
            baseAtt.RegenDelay -= Other.RegenDelay;
            //  baseAtt.CurrentValue = UARSFunctionLibrary::GetNewCurrentValueForNewMaxValue(CurrentValue, _oldmaxvalue, baseAtt.MaxValue);
        }
        return baseAtt;
    }

    FORCEINLINE FStatistic operator+(const FStatisticsModifier& Other) const
    {
        FStatistic baseAtt = *(this);
        const float oldmaxvalue = baseAtt.MaxValue;
        if (Other.AttributeType == this->StatType) {
            baseAtt.MaxValue += Other.MaxValue;
            baseAtt.RegenValue += Other.RegenValue;
            baseAtt.HasRegeneration = baseAtt.RegenValue != 0.f;
            //    baseAtt.CurrentValue = UARSFunctionLibrary::GetNewCurrentValueForNewMaxValue(CurrentValue, _oldmaxvalue, baseAtt.MaxValue);
        }
        return baseAtt;
    }

    FORCEINLINE FStatistic operator-(const FStatisticsModifier& Other) const
    {
        FStatistic baseAtt = *(this);
        const float oldmaxvalue = baseAtt.MaxValue;
        if (Other.AttributeType == this->StatType) {
            baseAtt.MaxValue -= Other.MaxValue;
            baseAtt.RegenValue -= Other.RegenValue;
            baseAtt.HasRegeneration = baseAtt.RegenValue != 0.f;
            //     baseAtt.CurrentValue = UARSFunctionLibrary::GetNewCurrentValueForNewMaxValue(CurrentValue, _oldmaxvalue, baseAtt.MaxValue);
        }
        return baseAtt;
    }

    FORCEINLINE bool operator==(const FGameplayTag& Other) const { return this->StatType == Other; }

    FORCEINLINE bool operator!=(const FGameplayTag& Other) const { return this->StatType != Other; }
};

USTRUCT(BlueprintType)
struct FAttribute {
    GENERATED_BODY()

    // default constructor
    FAttribute() { Value = 0.f; }

    FAttribute(const FGameplayTag& _stat, float _value)
    {
        Value = _value;
        AttributeType = _stat;
    }

    ~FAttribute() {};

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ARS)
    FGameplayTag AttributeType;

    // current stat value
    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ARS)
    float Value;

    FORCEINLINE FAttribute operator+(const FAttribute& Other) const
    {
        FAttribute baseAtt;
        baseAtt.AttributeType = this->AttributeType;
        baseAtt.Value = this->Value;
        if (Other.AttributeType == this->AttributeType) {
            baseAtt.Value += Other.Value;
        }
        return baseAtt;
    }

    FORCEINLINE FAttribute operator-(const FAttribute& Other) const
    {
        FAttribute baseAtt;
        baseAtt.AttributeType = this->AttributeType;
        baseAtt.Value = this->Value;
        if (Other.AttributeType == this->AttributeType) {
            baseAtt.Value -= Other.Value;
        }
        return baseAtt;
    }

    FORCEINLINE FAttribute operator+(const FAttributeModifier& Other) const
    {
        FAttribute baseAtt;
        baseAtt.AttributeType = this->AttributeType;
        baseAtt.Value = this->Value;
        if (Other.AttributeType == this->AttributeType) {
            baseAtt.Value += Other.Value;
        }
        return baseAtt;
    }

    FORCEINLINE FAttribute operator-(const FAttributeModifier& Other) const
    {
        FAttribute baseAtt;
        baseAtt.AttributeType = this->AttributeType;
        baseAtt.Value = this->Value;
        if (Other.AttributeType == this->AttributeType) {
            baseAtt.Value -= Other.Value;
        }
        return baseAtt;
    }

    FORCEINLINE bool operator==(const FAttribute& Other) const { return this->AttributeType == Other.AttributeType; }
    FORCEINLINE bool operator!=(const FAttribute& Other) const { return this->AttributeType != Other.AttributeType; }
    FORCEINLINE bool operator==(const FAttributeModifier& Other) const { return this->AttributeType == Other.AttributeType; }
    FORCEINLINE bool operator!=(const FAttributeModifier& Other) const { return this->AttributeType != Other.AttributeType; }
    FORCEINLINE bool operator==(const FGameplayTag& Other) const { return this->AttributeType == Other; }
    FORCEINLINE bool operator!=(const FGameplayTag& Other) const { return this->AttributeType != Other; }
};

USTRUCT(BlueprintType)
struct FAttributesSet : public FTableRowBase {
    GENERATED_BODY()

public:
    FAttributesSet() { }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Base Attributes", SaveGame, DisplayName = "PrimaryAttributes", meta = (TitleProperty = "AttributeType"))
    TArray<FAttribute> Attributes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Derived Attributes", SaveGame, DisplayName = "Statistics", meta = (TitleProperty = "StatType"))
    TArray<FStatistic> Statistics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Derived Attributes", SaveGame, DisplayName = "Attributes", meta = (TitleProperty = "AttributeType"))
    TArray<FAttribute> Parameters;

    ~FAttributesSet() {};
};

USTRUCT(BlueprintType)
struct FAttributesSetModifier {
    GENERATED_BODY()

public:
    // default constructor
    FAttributesSetModifier() { Guid = FGuid::NewGuid(); }

    UPROPERTY(BlueprintReadOnly, SaveGame, Category = ARS)
    FGuid Guid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Base Attributes", SaveGame)
    TArray<FAttributeModifier> PrimaryAttributesMod;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Derived Attributes", SaveGame)
    TArray<FStatisticsModifier> StatisticsMod;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARS | Derived Attributes", SaveGame)
    TArray<FAttributeModifier> AttributesMod;

    FORCEINLINE bool operator==(const FAttributesSetModifier& Other) const { return this->Guid == Other.Guid; }

    FORCEINLINE bool operator!=(const FAttributesSetModifier& Other) const { return this->Guid != Other.Guid; }

    ~FAttributesSetModifier() {};
};

USTRUCT(BlueprintType)
struct FStatisticValue {
    GENERATED_BODY()

public:
    FStatisticValue() {
        Statistic = FGameplayTag();
        Value = 0.f;
    };

    FStatisticValue(const FGameplayTag& tag, float value)
    {
        Statistic = tag;
        Value = value;
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ARS)
    FGameplayTag Statistic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ARS)
    float Value;
};

USTRUCT(BlueprintType)
struct FAttributeInfluence {
    GENERATED_BODY()

public:
    FAttributeInfluence() { CurveValue = nullptr; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    UCurveFloat* CurveValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    FGameplayTag TargetParameter;
};

USTRUCT(BlueprintType)
struct FStatInfluence {
    GENERATED_BODY()

public:
    FStatInfluence()
    {
        CurveRegenValue = nullptr;
        CurveMaxValue = nullptr;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    UCurveFloat* CurveRegenValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    UCurveFloat* CurveMaxValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    FGameplayTag TargetStat;
};

USTRUCT(BlueprintType)
struct FGenerationRule : public FTableRowBase {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FGameplayTag PrimaryAttributesTag;

    FORCEINLINE bool operator!=(const FGameplayTag& Other) const
    {
        return PrimaryAttributesTag != Other;
    }

    FORCEINLINE bool operator==(const FGameplayTag& Other) const
    {
        return PrimaryAttributesTag == Other;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    TArray<FStatInfluence> InfluencedStatistics;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ARS)
    TArray<FAttributeInfluence> InfluencedParameters;
};

USTRUCT(BlueprintType)
struct FAttributesByLevel {
    GENERATED_BODY()

public:

    FAttributesByLevel() {
        PrimaryAttributesTag = FGameplayTag();
        ValueByLevelCurve = nullptr;
    }
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FGameplayTag PrimaryAttributesTag;

    FORCEINLINE bool operator!=(const FGameplayTag& Other) const
    {
        return PrimaryAttributesTag != Other;
    }

    FORCEINLINE bool operator==(const FGameplayTag& Other) const
    {
        return PrimaryAttributesTag == Other;
    }

    /*A curve that has on the X axis the actual level of the character and on the Y
    axis the value of the related Primary Attribute*/
    UPROPERTY(EditDefaultsOnly, Category = ARS)
    UCurveFloat* ValueByLevelCurve;
};

USTRUCT(BlueprintType)
struct FTimedAttributeSetModifier {
    GENERATED_BODY()

public:
    FTimedAttributeSetModifier() {
        duration = 5.f;
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    FAttributesSetModifier modifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    float duration;
};

UCLASS()
class ADVANCEDRPGSYSTEM_API UARSTypes : public UObject {
    GENERATED_BODY()

public:
};
