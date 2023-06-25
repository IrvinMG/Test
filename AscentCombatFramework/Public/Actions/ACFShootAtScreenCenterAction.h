// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ACFShootAtScreenCenterAction.generated.h"
/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFShootAtScreenCenterAction : public UACFBaseAction {
    GENERATED_BODY()

public: 
    UACFShootAtScreenCenterAction();

    virtual bool CanExecuteAction_Implementation(class ACharacter* owner) override;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ShootAtScreenCenter(const float charge = 1.f);

protected:
    
	virtual void OnNotablePointReached_Implementation() override;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    float ShootTraceLength = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
    TEnumAsByte< ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TEnumAsByte<EDrawDebugTrace::Type> Debug;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
	TSubclassOf<class AACFProjectile> ProjectileOverride;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	FVector2D ScreenPosition;
};
