// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once


#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "ACFActionTypes.h"

#include "ACFMountableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMountedStateChanged, bool, inIsMounted);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class MOUNTSYSTEM_API UACFMountableComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFMountableComponent();

    friend class UACFRiderComponent;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = ACF)
    FName GetNearestMountPointTag(const FVector& location) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    class UACFMountPointComponent* GetNearestMountPoint(const FVector& location) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class APawn* GetPawnOwner() const
    {
        return pawnOwner;
    }

    UPROPERTY(EditAnywhere, Category = ACF)
    bool bPossessMount = true;

public:
    UFUNCTION( BlueprintCallable, Category = ACF)
    void StartMount(class ACharacter* inRider);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void StopMount(FName dismountPoint = NAME_None);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void TriggerActionOnRider(FGameplayTag ActionState, EActionPriority Priority = EActionPriority::ELow, bool bCanBeStored = false, const FString& contextString = "");

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void ReleaseSustainedActionOnRider(FGameplayTag actionTag);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FName GetMountPoint() const
    {
        return MountPointSocket;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FTransform GetMountPointTransform() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    class UACFMountPointComponent* GetDismountPoint(const FName& dismountPoint = NAME_None) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    UMeshComponent* GetMountMesh() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetMountMesh(UMeshComponent* inMountMeshOverride)
    {
        mountMeshOverride = inMountMeshOverride;
    }

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnMountedStateChanged OnMountedStateChanged;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsMounted() const
    {
        return bIsMounted;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetMountTag() const
    {
        return MountTypeTag;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetMountActionTag() const
    {
        return MountActionTag;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class ACharacter* GetRider() const
    {
        return rider;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class APawn* GetMountOwner() const
    {
        return GetPawnOwner();
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool NeedsPossession() const
    {
        return bPossessMount;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool CanBeMounted() const
    {
        return !rider;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    ETeam GetOwnerTeam() const;

protected:
    UPROPERTY(SaveGame, Replicated, ReplicatedUsing = OnRep_IsMounted)
    bool bIsMounted = false;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    FGameplayTag MountTypeTag;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    FGameplayTag MountActionTag;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    FName MountPointSocket;

    UPROPERTY(EditDefaultsOnly, Category = ACF)
    FName DefaultDismountPoint;

    virtual void SetMounted(bool inMounted);

    void SetRider(ACharacter* inRider)
    {
        rider = inRider;
    }

    UPROPERTY(Replicated)
    class ACharacter* rider;

private:

    UFUNCTION()
    void OnRep_IsMounted();

private:
    class APawn* pawnOwner;

    UMeshComponent* mountMeshOverride;
};
