// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "Actors/ACFCharacter.h"
#include "CoreMinimal.h"
#include <GameFramework/PlayerController.h>
#include <GameplayTagContainer.h>

#include "ACFPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPossessedCharacterChanged, const class AACFCharacter*, character);

UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFPlayerController : public APlayerController {
    GENERATED_BODY()

public:
    AACFPlayerController();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class AACFCharacter* GetPossessedACFCharacter() const { return PossessedCharacter; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FRotator GetCameraInput() const { return RotationInput; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetSecondsFromLastCameraInput() const { return secondsFromLastCameraInput; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE ETeam GetCombatTeam() const { return CombatTeam; }

    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = ACF)
    float GetXSensitivity() const;

    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = ACF)
    float GetYSensitivity() const;

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void SetCombatTeam(const ETeam& newTeam);

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnPossessedCharacterChanged OnPossessedCharacterChanged;

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void OnPossess(APawn* aPawn) override;

    void EnableCharacterComponents(bool bEnabled);

    virtual void OnUnPossess() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_PossessedEntity, Category = ACF)
    TObjectPtr<AACFCharacter> PossessedCharacter;

    IACFEntityInterface* PossessedEntity;

private:
    float secondsFromLastCameraInput = 0.f;

    FDateTime lastInput;

    UFUNCTION()
    void OnRep_PossessedEntity();

    void HandleNewEntityPossessed();
};
