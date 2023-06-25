// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Interfaces/ACFEntityInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Game/ACFDamageType.h"
#include "Interfaces/ACFInteractableInterface.h"
#include "ACFWheeledVehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLESYSTEM_API AACFWheeledVehiclePawn : public AWheeledVehiclePawn, 
					public IGenericTeamAgentInterface, public IACFEntityInterface,
					public IACFInteractableInterface
{
	GENERATED_BODY()

public:

	AACFWheeledVehiclePawn();

	//acf entity interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		ETeam GetEntityCombatTeam() const;
	virtual ETeam GetEntityCombatTeam_Implementation() const override
	{
		return CombatTeam;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		bool IsEntityAlive() const;
	virtual bool IsEntityAlive_Implementation() const override
	{
		return !bIsDead;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		float GetEntityExtentRadius() const;
	virtual float GetEntityExtentRadius_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		void AssignTeamToEntity(ETeam inCombatTeam);
	virtual void AssignTeamToEntity_Implementation(ETeam inCombatTeam) override;
	//END ACF ENTITY INTERFACE

	  //INTERACTION INTERFACE
	/* called when player interact with object of this class */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		bool CanBeInteracted(class APawn* Pawn);
	virtual bool CanBeInteracted_Implementation(class APawn* Pawn) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		FText GetInteractableName();
	virtual FText GetInteractableName_Implementation() override;
	//END INTERACTION INTERFACE

	UFUNCTION(BlueprintPure, Category = ACF)
	FACFDamageEvent GetLastDamageInfo() const;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnTeamChanged OnTeamChanged;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UARSStatisticsComponent* GetStatisticsComponent() const { return StatisticsComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFDamageHandlerComponent* GetDamageHandlerComponent() const { return DamageHandlerComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFMountableComponent* GetMountComponent() const { return MountComponent; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFMountPointComponent* GetDismountPoint() const { return DismountPoint; }

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnVehicleDestroyed();
	virtual void OnVehicleDestroyed_Implementation();


	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/*Used to identify who can attack this actor*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	ETeam CombatTeam = ETeam::ETeam1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UARSStatisticsComponent> StatisticsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UACFEffectsManagerComponent> EffetsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UACFDamageHandlerComponent> DamageHandlerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UACFMountableComponent> MountComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	TObjectPtr<class UACFMountPointComponent> DismountPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FName VehicleName = "SampleCar";

private:
	UPROPERTY(Replicated)
	bool bIsDead = false;

	UFUNCTION()
	void HandleDeath();
	
};
