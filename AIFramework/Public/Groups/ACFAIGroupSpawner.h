// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once


#include "ACFPatrolPath.h"
#include "CoreMinimal.h"
#include "Game/ACFTypes.h"
#include "GameFramework/Actor.h"
#include <GameFramework/Pawn.h>
#include <GameplayTagContainer.h>
#include <GenericTeamAgentInterface.h>
#include "ACFAITypes.h"
#include "ACFCoreTypes.h"

#include "ACFAIGroupSpawner.generated.h"

class USplineComponent;
class UACFGroupAIComponent;

UCLASS()
class AIFRAMEWORK_API AACFAIGroupSpawner : public APawn, public IGenericTeamAgentInterface, public IACFEntityInterface {
    GENERATED_BODY()

public:
    AACFAIGroupSpawner();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<USplineComponent> SplineComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<UACFGroupAIComponent> AIGroupComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    bool bSpawnOnBeginPlay = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FPatrolConfig PatrolConfig;
 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    bool bUpdateSpawnerPosition = true;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bUpdateSpawnerPosition"), BlueprintReadOnly, Category = ACF)
    float UpdateGroupPositionTimeInterval = 1.f;

    virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
    virtual FGenericTeamId GetGenericTeamId() const override;

    /// <summary>
    /// COMBAT ENTITY INTERFACE
    /// </summary>
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    ETeam GetEntityCombatTeam() const;
    virtual ETeam GetEntityCombatTeam_Implementation() const override
    {
        return GetCombatTeam();
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool IsEntityAlive() const;
    virtual bool IsEntityAlive_Implementation() const override
    {
        return GetGroupSize() > 0;
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void AssignTeamToEntity(ETeam inCombatTeam);
    virtual void AssignTeamToEntity_Implementation(ETeam inCombatTeam) override
    {
        return SetGenericTeamId(uint8(inCombatTeam));
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    float GetEntityExtentRadius() const;
    virtual float GetEntityExtentRadius_Implementation() const
    {
        return -1.f;
    }

    /// <summary>
    /// End Interface
    /// </summary>

public:
    UFUNCTION(BlueprintCallable, Category = ACF)
    FVector GetPatrolPoint(int32 patrolIndex) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    int32 GetGroupSize() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class USplineComponent* GetSplineComponent() const { return patrolPath->GetSplineComponent(); }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFGroupAIComponent* GetAIGroupComponent() const { return AIGroupComponent; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE ETeam GetCombatTeam() const { return CombatTeam; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetAgentWithIndex(int32 index, FAIAgentsInfo& outAgent) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    AACFCharacter* GetAgentNearestTo(const FVector& location) const;

private:
    UPROPERTY()
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY()
    TObjectPtr<class AACFPatrolPath> patrolPath;

    UFUNCTION()
    void StartUpdatePos();

    UFUNCTION()
    void StopUpdatePos();

    UFUNCTION()
    void AdjustSpawnerPos();

    bool bCheckPos = false;

    FTimerHandle timerHandle;
};
