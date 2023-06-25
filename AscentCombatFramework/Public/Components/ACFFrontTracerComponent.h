// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ACFFrontTracerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDetected,  const AActor*, detectedActor);


UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFFrontTracerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFFrontTracerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PerformTrace();

	UFUNCTION(BlueprintCallable, Category = ACF)
	AActor* PerformFrontTraceSingle();

	UFUNCTION(BlueprintCallable, Category = ACF)
	void StartContinuousTrace();
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	void StopContinuousTrace();

	UFUNCTION(BlueprintPure, Category = ACF)
	class AActor* GetCurrentlyTracedActor() const {
		return currentTracedActor;
	}

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnActorDetected OnNewActorDetected;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float TraceLength = 2500;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TArray<TEnumAsByte<EObjectTypeQuery>> ChannelsToTrace;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TEnumAsByte<EDrawDebugTrace::Type> ShowDebug;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TSubclassOf<AActor> ActorToFind;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bIgnorePlayer = true;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bShowDebugTrace = false;

private: 

	bool bCurrentTraceState;

	UPROPERTY()
	AActor* currentTracedActor;

	void SetCurrentTracedActor(AActor* newActor);
};
