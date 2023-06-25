// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ACFCoreTypes.h"

#include "ACFTeamsConfigDataAsset.generated.h"


struct FTeamInfo;

/**
 *
 */
const int32 MaxTeam = 5;


UCLASS()
class ASCENTCOREINTERFACES_API UACFTeamsConfigDataAsset : public UPrimaryDataAsset {
    GENERATED_BODY()

public:
    UACFTeamsConfigDataAsset();

    TMap<ETeam, FTeamInfo> GetTeamsConfig() const {
        return Teams;
     }

      int32 GetMaxTeams() const
     {
         return MaxTeam;
     }
     
protected: 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = ACF)
    TMap<ETeam, FTeamInfo> Teams;
};
