// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 


#include "ACFTeamsConfigDataAsset.h"


 UACFTeamsConfigDataAsset::UACFTeamsConfigDataAsset()
{
     // Default teams
     for (uint8 i = 0; i < MaxTeam; i++) {
         FTeamInfo TeamInfo = FTeamInfo();

         ETeam CurrentTeam = static_cast<ETeam>(i);
         TeamInfo.DisplayName = FText::FromString(FString::Printf(TEXT("Team %d"), (i + 1)));

         for (uint8 j = 0; j < MaxTeam; j++) {

             const ETeam TargetTeam = static_cast<ETeam>(j);
             ETeamAttitude::Type Attitude;

             if (CurrentTeam == ETeam::ENeutral) {
                 Attitude = ETeamAttitude::Neutral;
             } else {
                 if (CurrentTeam == TargetTeam) {
                     Attitude = ETeamAttitude::Friendly;
                 } else if (TargetTeam == ETeam::ENeutral) {
                     Attitude = ETeamAttitude::Neutral;
                 } else {
                     Attitude = ETeamAttitude::Hostile;
                 }
             }

             TeamInfo.Relationship.Add(TargetTeam, Attitude);
         }

         Teams.Add(CurrentTeam, TeamInfo);
     }
 }
