// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ACFHUD.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFHUD : public AHUD
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = ACF)
	TSubclassOf<class UACFHUDUserWidget> HUDClass;

	UPROPERTY(BlueprintReadWrite, Category = ACF)
	class UACFHUDUserWidget* HUDWidget;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitProperties() override;

public: 


	UFUNCTION(BlueprintCallable, Category = ACF)
	void EnableCrosshair(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = ACF)
    void SetHudEnabled(bool bEnabled);
	
private: 

	UFUNCTION()
	void HandleCharacterPossessedChanged(const class AACFCharacter* newChar);

	/*UFUNCTION(BlueprintCallable, Category = ACF)*/
	void InitHUD();
};
