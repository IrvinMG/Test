// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Commands/ACFBaseCommand.h"
#include "ACFGoThereCommand.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AIFRAMEWORK_API UACFGoThereCommand : public UACFBaseCommand
{
	GENERATED_BODY()
protected: 

	UPROPERTY(EditDefaultsOnly, Category = ACF)
		bool bDrawDebug ;
	
public: 

	virtual void ExecuteCommand_Implementation(class AACFAIController* controller) override;

	virtual void EndCommand_Implementation(class AACFAIController* controller) override;
};
