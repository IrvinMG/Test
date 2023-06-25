// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACFAIPatrolComponent.generated.h"


UCLASS( ClassGroup=(ACF), meta=(BlueprintSpawnableComponent) )
class AIFRAMEWORK_API UACFAIPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFAIPatrolComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	class AACFPatrolPath* PathToFollow;

public:	

	UFUNCTION(BlueprintCallable, Category = ACF)
	class AACFPatrolPath* GetPathToFollow() const { return PathToFollow; }

	void SetPathToFollow(class AACFPatrolPath* val) { PathToFollow = val; }
};
