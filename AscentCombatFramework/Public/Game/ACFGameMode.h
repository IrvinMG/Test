// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ACFDamageType.h"
#include "ACFGameMode.generated.h"

/**
 * 
 */


UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AACFGameMode();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFRagdollMasterComponent* GetRagdollManager() const { return RagdollManager; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACMCollisionsMasterComponent* GetCollisionsManager() const { return CollisionManager; }
// 
// 	UFUNCTION(BlueprintCallable, Category = ACF)
// 	void SpawnPlayersCompanions();

	UFUNCTION(BlueprintPure, Category = ACF)
	TArray<class AACFPlayerController*> GetAllPlayerControllers();
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFRagdollMasterComponent* RagdollManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACMCollisionsMasterComponent* CollisionManager;

	
};
