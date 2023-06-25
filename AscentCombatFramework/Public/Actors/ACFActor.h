// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ACFEntityInterface.h"
#include <GenericTeamAgentInterface.h>
#include "ACFActor.generated.h"


UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFActor : public AActor,
                                            public IGenericTeamAgentInterface,
                                            public IACFEntityInterface {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AACFActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /*Used to identify who can attack this actor*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UARSStatisticsComponent> StatisticsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFEffectsManagerComponent> EffetsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFEquipmentComponent> EquipmentComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UACFDamageHandlerComponent> DamageHandlerComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UAudioComponent> AudioComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UPrimitiveComponent> RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActorDestroyed();
    virtual void OnActorDestroyed_Implementation();


public:
    virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

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

    UFUNCTION(BlueprintPure, Category = ACF)
    FACFDamageEvent GetLastDamageInfo() const;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnTeamChanged OnTeamChanged;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UARSStatisticsComponent* GetStatisticsComponent() const { return StatisticsComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFEquipmentComponent* GetEquipmentComponent() const { return EquipmentComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFDamageHandlerComponent* GetDamageHandlerComponent() const { return DamageHandlerComp; }

private:
    UPROPERTY(Replicated)
    bool bIsDead = false;

    UFUNCTION()
    void HandleDeath();
};
