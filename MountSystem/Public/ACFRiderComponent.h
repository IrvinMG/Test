// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "ACFRiderComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRidingStateChanged, bool, bIsRiding);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class MOUNTSYSTEM_API UACFRiderComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFRiderComponent();

    UPROPERTY(BlueprintAssignable)
    FOnRidingStateChanged OnRidingStateChanged;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsRiding() const { return bIsRiding; }

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void StartMount(class UACFMountableComponent* mount);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void StartDismount(const FName& dismountPoint = NAME_None);

    UFUNCTION(BlueprintPure, Category = ACF)
    class APawn* GetMount() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFMountableComponent* GetMountComp() const {
        return Mount;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetMountTypeTag() const;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(SaveGame, Replicated, ReplicatedUsing = OnRep_IsRiding)
    bool bIsRiding = false;

private:
    void HandlePossession();
    void FinishDismount(const FName& dismountPoint = NAME_None);

    void Internal_DetachFromMount();

    UFUNCTION()
    void OnRep_Mount();

    UPROPERTY(ReplicatedUsing = OnRep_Mount)
    class UACFMountableComponent* Mount;

    UPROPERTY()
    class ACharacter* charOwner;

    UFUNCTION()
    void OnRep_IsRiding();

    void Internal_Ride(bool inIsRiding);

    void Internal_AttachToMount();

    void Internal_Mount();

    void Internal_SetMountCollisionsEnabled(const bool bMounted);
};
