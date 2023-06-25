// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFActionCondition.h"
#include "ACFCCTypes.h"
#include "Actors/ACFCharacter.h"
#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>

#include "ACFAITypes.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FPatrolConfig {
    GENERATED_BODY()

public:

    FPatrolConfig() {
        ReversePath = true;
        WaitTimeAtPatrolPoint = 0.f;
    }
    /*If true once the need of the path is reached, those AIs will try to come
    back to the original point */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    bool ReversePath;

    /*Amount of time this AI will stay at the patrol point*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float WaitTimeAtPatrolPoint;
};

UENUM(BlueprintType)
enum class EAgentRotation : uint8 {
    EGroupForward = 0 UMETA(DisplayName = "Group Forward"),
    EGroupCenter = 1 UMETA(DisplayName = "Look at Group Center")
};

UENUM(BlueprintType)
enum class EAIState : uint8 {
    EPatrol = 0 UMETA(DisplayName = "Patrol"),
    EBattle = 1 UMETA(DisplayName = "Combat"),
    EFollowLeader = 2 UMETA(DisplayName = "Follow Lead Actor"),
    EReturnHome = 3 UMETA(DisplayName = "Return Home"),
    EWait = 4 UMETA(DisplayName = "Wait"),
};

UENUM(BlueprintType)
enum class EAICombatState : uint8 {
    EMeleeCombat = 0 UMETA(DisplayName = "Melee Attacks"),
    EChaseTarget = 1 UMETA(DisplayName = "Chase Target"),
    ERangedCombat = 2 UMETA(DisplayName = "Ranged Attacks"),
    EStudyTarget = 3 UMETA(DisplayName = "Studytarget"),
    EFlee = 4 UMETA(DisplayName = "Flee Away"),  
};

USTRUCT(BlueprintType)
struct FAIAgentsInfo {
    GENERATED_BODY()

public:
    FAIAgentsInfo()
    {
        AICharacter = nullptr;
        AIController = nullptr;
       
        // characterClass = AACFCharacter::StatiClass();
    };

    FAIAgentsInfo(class AACFCharacter* inChar, class AACFAIController* inContr);

     FAIAgentsInfo(class AACFCharacter* inCharr);

    FORCEINLINE bool operator==(const FAIAgentsInfo& Other) const
    {
        return this->AICharacter == Other.AICharacter;
    }

    FORCEINLINE bool operator!=(const FAIAgentsInfo& Other) const
    {
        return this->AICharacter != Other.AICharacter;
    }

    FORCEINLINE bool operator==(const AACFCharacter* Other) const
    {
        return this->AICharacter == Other;
    }

    FORCEINLINE bool operator!=(const AACFCharacter* Other) const
    {
        return this->AICharacter != Other;
    }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr <class AACFCharacter> AICharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class AACFAIController> AIController;

    UPROPERTY(SaveGame)
    TSubclassOf<AACFCharacter> characterClass;

    UPROPERTY(SaveGame)
    FString Guid;
};

USTRUCT(BlueprintType)
struct FCommandChances : public FTableRowBase {
    GENERATED_BODY()

public:
    FCommandChances()
    {
        Condition = nullptr;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FGameplayTag CommandTag;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = ACF)
    class UACFActionCondition* Condition;
};

USTRUCT(BlueprintType)
struct FAICombatStateConfig {
    GENERATED_BODY()

public:
    FAICombatStateConfig()
    {
        Conditions.Add(NewObject<UACFDistanceActionCondition>());
        CombatState = EAICombatState::EMeleeCombat;
    };

    FAICombatStateConfig(const EAICombatState inCombatState, float inChance, const ELocomotionState& inState)
    {
        TriggerChancePercentage = inChance;
        LocomotionState = inState;
        Conditions.Add(NewObject<UACFDistanceActionCondition>());
        CombatState = inCombatState;
    }
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    EAICombatState CombatState;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = ACF)
    TArray<class UACFActionCondition*> Conditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    float TriggerChancePercentage = 75.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    ELocomotionState LocomotionState = ELocomotionState::EWalk;

    UACFDistanceActionCondition* GetDistanceBasedCondition() const
    {
        for (const auto condition : Conditions) {
            UACFDistanceActionCondition* distanceCond = Cast<UACFDistanceActionCondition>(condition);
            if (distanceCond) {
                return distanceCond;
            }
        }
        return nullptr;
    }

    FORCEINLINE bool operator==(const EAICombatState& Other) const
    {
        return this->CombatState == Other;
    }

    FORCEINLINE bool operator!=(const EAICombatState& Other) const
    {
        return this->CombatState != Other;
    }

    FORCEINLINE bool operator==(const FAICombatStateConfig& Other) const
    {
        return this->CombatState == Other.CombatState;
    }

    FORCEINLINE bool operator!=(const FAICombatStateConfig& Other) const
    {
        return this->CombatState != Other.CombatState;
    }
};

USTRUCT(BlueprintType)
struct FConditions : public FActionChances {
    GENERATED_BODY()

public:
    FConditions()
    {
        Condition = nullptr;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = ACF)
    class UACFActionCondition* Condition;
};

USTRUCT(BlueprintType)
struct FAISpawnInfo {
    GENERATED_BODY()

    FAISpawnInfo()
    {
        AIClassBP = AACFCharacter::StaticClass();
        SpawnRotation = EAgentRotation::EGroupForward;
    };

    FAISpawnInfo(const TSubclassOf<class AACFCharacter> classToSpawn)
    {
        AIClassBP = classToSpawn;
        SpawnRotation = EAgentRotation::EGroupForward;
    }

       FAISpawnInfo(const TSubclassOf<class AACFCharacter> classToSpawn, const int32 totalNum)
    {
        AIClassBP = classToSpawn;
        SpawnRotation = EAgentRotation::EGroupForward;
        TotalNumber = totalNum;
    }

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ACF)
    TSubclassOf<AACFCharacter> AIClassBP;

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ACF)
    EAgentRotation SpawnRotation;

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ACF)
    /** the number of this ai class in group*/
    int32 TotalNumber = 1;

    FORCEINLINE bool operator==(const FAISpawnInfo& Other) const
    {
        return this->AIClassBP == Other.AIClassBP;
    }

    FORCEINLINE bool operator!=(const FAISpawnInfo& Other) const
    {
        return this->AIClassBP != Other.AIClassBP;
    }

    FORCEINLINE bool operator==(const TSubclassOf<AACFCharacter>& Other) const
    {
        return this->AIClassBP == Other;
    }

    FORCEINLINE bool operator!=(const TSubclassOf<AACFCharacter>& Other) const
    {
        return this->AIClassBP != Other;
    }
};

USTRUCT(BlueprintType)
struct FWaveConfig {
    GENERATED_BODY()

public:

    FWaveConfig() {
        GroupSpawner = nullptr;
        secondsDelayToNextWave = 1.f;
    }
    /*The spawner to be used for this wave */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TSoftObjectPtr<class AACFAIGroupSpawner> GroupSpawner;

    /*The Agents To Be Used For This Wave. Overrides default spawner configuration*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    TArray<FAISpawnInfo> WaveAgentsOverride;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float secondsDelayToNextWave;

    void AddAIToSpawn(const TSubclassOf<AACFCharacter>& charClass, int32 totalNumber = 1)
    {
        if (WaveAgentsOverride.Contains(charClass)) {
            WaveAgentsOverride.FindByKey(charClass)->TotalNumber += totalNumber;
        } else {
            WaveAgentsOverride.Add(FAISpawnInfo(charClass, totalNumber));
        }
    }

    void RemoveAIToSpawn(const TSubclassOf<AACFCharacter>& charClass, int32 totalNumber = 1)
    {
        if (WaveAgentsOverride.Contains(charClass)) {
            const int32 aiNum = WaveAgentsOverride.FindByKey(charClass)->TotalNumber -= totalNumber;
            if (aiNum <= 0) {
                WaveAgentsOverride.Remove(charClass);
            }
        }
    }


};

UCLASS()
class AIFRAMEWORK_API UACFAITypes : public UObject {
    GENERATED_BODY()
};
