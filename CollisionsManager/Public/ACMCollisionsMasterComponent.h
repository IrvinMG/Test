// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMCollisionsMasterComponent.generated.h"


UCLASS(ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class COLLISIONSMANAGER_API UACMCollisionsMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACMCollisionsMasterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddComponent(class UACMCollisionManagerComponent* compToAdd);

	void RemoveComponent(class UACMCollisionManagerComponent* compToAdd);

private:

	UPROPERTY()
	TArray<class UACMCollisionManagerComponent*> currentlyActiveComponents;

	UPROPERTY()
	TArray<class UACMCollisionManagerComponent*> pendingDelete;
};
