// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFSummonAction.generated.h"

/**
 * 
 */
UCLASS()
class SPELLACTIONS_API UACFSummonAction : public UACFBaseAction
{
	GENERATED_BODY()

protected:

	virtual bool CanExecuteAction_Implementation(class ACharacter* owner) override;

	virtual void OnNotablePointReached_Implementation() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF)
	TSubclassOf<class AACFCharacter> CompanionToSummonClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF)
	uint8 MaxCompanionNumb = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF)
	float SpawnRadius = 350.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = ACF)
	bool bAutoKillAfterSeconds = true;

	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = bAutoKillAfterSeconds), EditAnywhere, Category = ACF)
	float AutoKillTime = 30.f;

private: 

	TArray<class AACFCharacter*> Companions;

	UFUNCTION()
	void KillCompanion(class AACFCharacter* comp);

	UFUNCTION()
	void OnCompanionDeath();

	uint8 currentCompIndex = 0;
};
