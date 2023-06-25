// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ACFActionTypes.h"
#include "Engine/DataAsset.h"
#include "ACFActionsSet.generated.h"
/**
 *
 */
UCLASS(BlueprintType, Blueprintable, Category = ACF)
class ACTIONSSYSTEM_API UACFActionsSet : public UPrimaryDataAsset {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "ACF | Actions")
     TArray<FActionState> Actions;

public:

    void AddOrModifyAction(const FActionState& action);

	bool GetActionByTag(const FGameplayTag& action, FActionState& outAction) const;

	void GetActions(TArray<FActionState>& outActions) const {
		outActions = Actions;
	}
};
