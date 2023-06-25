// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ACFUpdateStateBTService.generated.h"

/**
 * 
 */
UCLASS()
class AIFRAMEWORK_API UACFUpdateStateBTService : public UBTService
{
	GENERATED_BODY()

protected:


	UPROPERTY(BlueprintReadOnly, Category = ACF)
	TObjectPtr<class AACFAIController>  aiController;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
    TObjectPtr < class APawn> CharOwner;

public:
	
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
