// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/ACFTypes.h"
#include "ACFAITypes.h"
#include "ACFPatrolPath.generated.h"

UCLASS()
class AIFRAMEWORK_API AACFPatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACFPatrolPath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = ACF)
	class USplineComponent* SplineComp;

	UPROPERTY(EditAnywhere, Category = ACF)
	FPatrolConfig PatrolConfig;

public:	

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return SplineComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FPatrolConfig GetPatrolConfig() const { return PatrolConfig; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetPatrolConfig(const FPatrolConfig& inPatrolConfig){
		PatrolConfig = inPatrolConfig;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	void AddSplinePoint(const FVector& worldPos);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetSplinePoints(const TArray<FVector>& worldPos);

};
