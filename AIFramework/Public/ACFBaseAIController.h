// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "AIController.h"
#include "CoreMinimal.h"
#include "Interfaces/ACFEntityInterface.h"
#include "GameFramework/Pawn.h"
#include "ACFCoreTypes.h"
#include "ACFBaseAIController.generated.h"
/**
 * 
 */
UCLASS()
class AIFRAMEWORK_API AACFBaseAIController : public AAIController {
    GENERATED_BODY()

protected:
    /*Combat team of the controlled ai*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ACF ")
    ETeam CombatTeam = ETeam::ETeam1;

	virtual void OnPossess(APawn* _possPawn) override;

	virtual void OnUnPossess() override;

	IACFEntityInterface* PossessedEntity;

public: 

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE ETeam GetCombatTeam()const { return CombatTeam; }


	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
	void SetCombatTeam(const ETeam& newTeam);


	/// <summary>
	/// /Combat Team Interface
	/// </summary>
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	/// <summary>
	/// /End Combat Team Entity
	/// </summary>



};
