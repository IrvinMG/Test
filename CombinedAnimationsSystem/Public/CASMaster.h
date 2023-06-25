// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CASMaster.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBINEDANIMATIONSSYSTEM_API UCASMaster : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCASMaster();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = CAS)
	void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, class ACharacter* character);
		
};
