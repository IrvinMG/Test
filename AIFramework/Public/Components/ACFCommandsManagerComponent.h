// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "Containers/Queue.h"
#include "ACFCommandsManagerComponent.generated.h"

class UACFBaseCommand;

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class AIFRAMEWORK_API UACFCommandsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFCommandsManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadWrite, Category = ACF)
	TMap<FGameplayTag, class UACFBaseCommand*> commands;

	TQueue<class UACFBaseCommand*> commandToExecute;

public:	

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
	void ExecutePendingCommand();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
	void TriggerCommand(const FGameplayTag& command);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void TerminateCurrentCommand();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool HasPendingCommands()const { return !commandToExecute.IsEmpty(); }

private: 

	UPROPERTY()
	class AACFAIController* controllerOwner;

	UPROPERTY()
	class UACFBaseCommand* currentCommand;
};
