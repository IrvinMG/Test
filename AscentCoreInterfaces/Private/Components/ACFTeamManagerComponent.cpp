// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFTeamManagerComponent.h"
#include "Engine/CollisionProfile.h"
#include "ACFTeamsConfigDataAsset.h"

// Sets default values for this component's properties
UACFTeamManagerComponent::UACFTeamManagerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UACFTeamManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::Internal_GetEnemiesCollisionChannels(const ETeam SelfTeam) const
{
    TArray<TEnumAsByte<ECollisionChannel>> channels;

    for (uint8 index = 0; index < MaxTeam; index++) {
        const ETeam TargetTeam = static_cast<ETeam>(index);

        if (IsEnemyTeam(SelfTeam, TargetTeam)) {
            channels.Add(GetCollisionChannelByTeam(TargetTeam, true));
            channels.Add(GetCollisionChannelByTeam(TargetTeam, false));
        }
    }
    return channels;
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::GetAllCollisionChannels(bool bIgnoreNeutral) const
{
    TArray<TEnumAsByte<ECollisionChannel>> channels;

    for (int32 i = 0; i < MaxTeam; i++) {
        const ETeam TargetTeam = static_cast<ETeam>(i);
        if (TargetTeam == ETeam::ENeutral && bIgnoreNeutral) {
            continue;
        }
        channels.Add(GetCollisionChannelByTeam(TargetTeam));
    }
    return channels;
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::GetEnemiesCollisionChannels(const ETeam SelfTeam) const
{
    if (BattleType == EBattleType::EEveryoneAgainstEveryone) {
        return GetAllCollisionChannels(true);
    } else {
        return Internal_GetEnemiesCollisionChannels(SelfTeam);
    }
    return TArray<TEnumAsByte<ECollisionChannel>>();
}

TEnumAsByte<ECollisionChannel> UACFTeamManagerComponent::GetCollisionChannelByTeam(ETeam Team, bool getBlockinChannel) const
{
  
    if (TeamsConfiguration) {
        const TMap<ETeam, FTeamInfo> Teams = TeamsConfiguration->GetTeamsConfig();
        if (Teams.Contains(Team)) {
            if (getBlockinChannel) {
                return Teams.Find(Team)->BlockingCollisionChannel;
            } else {
                return Teams.Find(Team)->CollisionChannel;
            }
        }
    }

    UE_LOG(LogTemp, Error, TEXT("INVALID TEAM! - UACFTeamManagerComponent "));
    return ECollisionChannel::ECC_MAX;
}

bool UACFTeamManagerComponent::IsEnemyTeam(ETeam SelfTeam, ETeam TargetTeam) const
{
    if (BattleType == EBattleType::EEveryoneAgainstEveryone) {
        return true;
    }

     if (TeamsConfiguration) {
        const TMap<ETeam, FTeamInfo> Teams = TeamsConfiguration->GetTeamsConfig();
        if (Teams.Contains(SelfTeam)) {
            const FTeamInfo* teamInfo = Teams.Find(SelfTeam);
            if (teamInfo && teamInfo->Relationship.Contains(TargetTeam)) {
                return teamInfo->Relationship.Find(TargetTeam)->GetValue() == ETeamAttitude::Hostile;
            }
        }
    }

    UE_LOG(LogTemp, Error, TEXT("INVALID TEAM RELATIONSHIPS CONFIG! CHECK GAME STATE- UACFTeamManagerComponent "));

    return false;
}
