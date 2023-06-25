// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFCoreTypes.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ACFTeamManagerComponent.generated.h"

UCLASS(ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOREINTERFACES_API UACFTeamManagerComponent : public UActorComponent {
    GENERATED_BODY()

    // Constructors
public:
    // Sets default values for this component's properties
    UACFTeamManagerComponent();
    // Methods
public:
    TArray<TEnumAsByte<ECollisionChannel>> GetAllCollisionChannels(const bool bIgnoreNeutral) const;
    TArray<TEnumAsByte<ECollisionChannel>> GetEnemiesCollisionChannels(const ETeam SelfTeam) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    TEnumAsByte<ECollisionChannel> GetCollisionChannelByTeam(ETeam Team, bool getBlockinChannel = false) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsEnemyTeam(ETeam SelfTeam, ETeam TargetTeam) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE EBattleType GetBattleType() const { return BattleType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFTeamsConfigDataAsset* GetTeamsConfiguration() const { return TeamsConfiguration; }

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    EBattleType BattleType = EBattleType::ETeamBased;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    class UACFTeamsConfigDataAsset* TeamsConfiguration;

private:
    TArray<TEnumAsByte<ECollisionChannel>> Internal_GetEnemiesCollisionChannels(const ETeam SelfTeam) const;
};
