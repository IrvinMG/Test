// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFCCTypes.h"
#include "Animation/ACFAnimTypes.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Interfaces/ACFInteractableInterface.h"
#include <GenericTeamAgentInterface.h>

#include "ACFCharacter.generated.h"

class USkeletalMeshComponent;

enum class EActionPriority : uint8;
enum class EDamageZone : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterFullyInitialized);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageReceived, const FACFDamageEvent&, damageReceived);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageInflicted, class AActor*, damageReceiver);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrouchStateChanged, bool, isCrouched);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovesetChanged, const FGameplayTag&, Moveset);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatTypeChanged, ECombatType, CombatType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathEvent, AACFCharacter*, self);

UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFCharacter : public ACharacter,
                                                public IGenericTeamAgentInterface,
                                                public IACFEntityInterface {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AACFCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void PostInitProperties() override;

    virtual void EndPlay(EEndPlayReason::Type reason) override;
    void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker);
    virtual void PreInitializeComponents() override;
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor) override;

    virtual void OnMovementModeChanged(EMovementMode prevMovementMde, uint8 PreviousCustomMode = 0) override;

    virtual void Landed(const FHitResult& Hit) override;

    /*Name Displayed in UI*/
    UPROPERTY(EditDefaultsOnly, SaveGame, Category = "ACF")
    FText CharacterName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF")
    TMap<FName, EDamageZone> BoneNameToDamageZoneMap;

    UPROPERTY(EditAnywhere, Category = "ACF | Fall")
    float FallDamageDistanceThreshold = 200.f;

    UPROPERTY(EditAnywhere, Category = "ACF | Fall")
    class UCurveFloat* FallDamageByFallDistance;

    UPROPERTY(EditAnywhere, Category = "ACF | Fall")
    bool bTriggerActionOnLand = true;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bTriggerActionOnLand"), Category = "ACF | Fall")
    float FallHeightToTriggerAction = 300.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bTriggerActionOnLand"), Category = "ACF | Fall")
    FGameplayTag ActionsToTriggerOnLand;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Actions Comp", Category = ACF)
    TObjectPtr<class UACFActionsManagerComponent> ActionsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Character Movement Comp", Category = ACF)
    TObjectPtr<class UACFCharacterMovementComponent> LocomotionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Statistics Comp", Category = ACF)
    TObjectPtr<class UARSStatisticsComponent> StatisticsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Collisions ManagerComp", Category = ACF)
    TObjectPtr<class UACMCollisionManagerComponent> CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Equipment & Inventory Comp", Category = ACF)
    TObjectPtr<class UACFEquipmentComponent> EquipmentComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF VFX & SFX Comp", Category = ACF)
    TObjectPtr<class UACFEffectsManagerComponent> EffetsComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Damage Handler Comp", Category = ACF)
    TObjectPtr<class UACFDamageHandlerComponent> DamageHandlerComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "ACF Ragdoll Comp", Category = ACF)
    TObjectPtr<class UACFRagdollComponent> RagdollComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UMotionWarpingComponent> MotionWarpComp;

    UPROPERTY(VisibleAnywhere, Category = ACF)
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
    TObjectPtr<class UAudioComponent> AudioComp;

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    float GetBaseDamage();
    virtual float GetBaseDamage_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnCharacterDeath();
    virtual void OnCharacterDeath_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    USkeletalMeshComponent* GetMainMesh() const;
    virtual USkeletalMeshComponent* GetMainMesh_Implementation() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetFallDamageFromDistance(float fallDistance);

    UFUNCTION(NetMulticast, Reliable, WithValidation, Category = ACF)
    void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None, float rootMotionScale = 1.f);

    virtual void PostInitializeComponents() override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "ACF")
    ECombatType CombatType = ECombatType::EUnarmed;

    UPROPERTY(EditDefaultsOnly, Category = "ACF | Death")
    EDeathType DeathType = EDeathType::EDeathAction;

    UPROPERTY(EditDefaultsOnly, Category = "ACF | Death")
    bool bAutoDestroyOnDeath = false;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bAutoDestroyOnDeath"), Category = "ACF | Death")
    float DestroyTimeOnDeath = 2.f;

    UPROPERTY(EditDefaultsOnly, Category = "ACF | Death")
    bool bDisableCapsuleOnDeath = true;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsImmortal = false;

    /*Team agent interface*/
    virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
    virtual FGenericTeamId GetGenericTeamId() const override;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void TryJump();

private:
    bool bInitialized = false;

    float ZFalling = -1.f;

    UFUNCTION()
    void HandleDamageReceived(const FACFDamageEvent& damageReceived);

    UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
    void ServerSendPlayMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None, float rootMotionScale = 1.f);

    UFUNCTION()
    void SetupCollisionManager();

    UFUNCTION(NetMulticast, Reliable, Category = ACF)
    void ClientsSwitchMovetype(const FGameplayTag& moveType);

    UFUNCTION(NetMulticast, Reliable, Category = ACF)
    void ClientsSwitchOverlay(const FGameplayTag& overlayTag);

    void Internal_SwitchMovetype(const FGameplayTag& moveAction);

    UFUNCTION(NetMulticast, Reliable, Category = ACF)
    void ClientsOnCharacterDeath();

    UFUNCTION()
    void HandleCharacterDeath();

    UFUNCTION()
    void InitializeCharacter();

    UFUNCTION()
    void HandleEquipmentChanged(const FEquipment& equipment);

    UPROPERTY(Replicated)
    ETeam CombatTeam = ETeam::ETeam1;

    UFUNCTION()
    void OnRep_ReplicatedAcceleration();

    UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
    FReplicatedAcceleration ReplicatedAcceleration;

public:
    void AssignTeam(ETeam team);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetIsImmortal(bool inImmortal)
    {
        bIsImmortal = inImmortal;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsImmortal() const
    {
        return bIsImmortal;
    }

    virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    virtual void Crouch(bool bClientSimulation = false) override;

    virtual void UnCrouch(bool bClientSimulation = false) override;

    // SAVABLE INTERFACE
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnActorLoaded();
    virtual void OnActorLoaded_Implementation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnActorSaved();
    virtual void OnActorSaved_Implementation();

    /// <summary>
    /// COMBAT ENTITY INTERFACE
    /// </summary>
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    ETeam GetEntityCombatTeam() const;
    virtual ETeam GetEntityCombatTeam_Implementation() const override
    {
        return GetCombatTeam();
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool IsEntityAlive() const;
    virtual bool IsEntityAlive_Implementation() const override
    {
        return IsAlive();
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void AssignTeamToEntity(ETeam inCombatTeam);
    virtual void AssignTeamToEntity_Implementation(ETeam inCombatTeam) override
    {
        return AssignTeam(inCombatTeam);
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    float GetEntityExtentRadius() const;
    virtual float GetEntityExtentRadius_Implementation() const
    {
        return GetCapsuleComponent()->GetScaledCapsuleRadius();
    }

    /// <summary>
    /// End Interface
    /// </summary>

    UFUNCTION(BlueprintPure, Category = ACF)
    ETeam GetCombatTeam() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool IsMyEnemy(AACFCharacter* target) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    EDeathType GetDeathType() const
    {
        return DeathType;
    }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetDeathType(EDeathType inDeathType)
    {
        DeathType = inDeathType;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    ECollisionChannel GetCollisionChannel() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    EDamageZone GetDamageZoneByBoneName(FName BoneName) const;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnDamageReceived OnDamageReceived;

    UFUNCTION(BlueprintPure, Category = ACF)
    TArray<TEnumAsByte<ECollisionChannel>> GetEnemiesCollisionChannel() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsRanged() const
    {
        return CombatType == ECombatType::ERanged;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    bool CanBeRanged() const;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnMovesetChanged OnMovesetChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnCombatTypeChanged OnCombatTypeChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnDamageInflicted OnDamageInflicted;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnCharacterFullyInitialized OnCharacterFullyInitialized;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnTeamChanged OnTeamChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnCrouchStateChanged OnCrouchStateChanged;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnDeathEvent OnDeath;

    UFUNCTION(BlueprintPure, Category = ACF)
    float GetWalkSpeed() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    float GetJogSpeed() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    float GetSprintSpeed() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void TriggerAction(FGameplayTag Action, EActionPriority Priority, bool canBeStored = false, const FString& contextString = "");

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ForceActionByName(FName ActionName);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ForceAction(FGameplayTag Action);

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetCurrentActionState() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    AActor* GetTarget() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetCharacterName(const FText& inCharacterName)
    {
        CharacterName = inCharacterName;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFCharacterMovementComponent* GetACFCharacterMovementComponent() const { return LocomotionComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFActionsManagerComponent* GetActionsComponent() const { return ActionsComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UARSStatisticsComponent* GetStatisticsComponent() const { return StatisticsComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFEquipmentComponent* GetEquipmentComponent() const { return EquipmentComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACMCollisionManagerComponent* GetCollisionsComponent() const { return CollisionComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFDamageHandlerComponent* GetDamageHandlerComponent() const { return DamageHandlerComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UACFRagdollComponent* GetRagdollComponent() const { return RagdollComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UMotionWarpingComponent* GetMotionWarpComponent() const { return MotionWarpComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    class UATSBaseTargetComponent* GetTargetingComponent() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    bool IsAlive() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FText GetCharacterName() const { return CharacterName; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE class UAudioComponent* GetAudioComp() const { return AudioComp; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE ECombatType GetCombatType() const { return CombatType; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FGameplayTag GetCurrentMoveset() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFAnimInstance* GetACFAnimInstance() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetCurrentMaxSpeed() const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FACFDamageEvent GetLastDamageInfo() const;

    UFUNCTION()
    void HandleAttackHit(const FHitResult& HitResult);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool GetIsDead() const { return !IsAlive(); }

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void KillCharacter();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void ReviveCharacter(float normalizedHealthToGrant = 1.f);

    UFUNCTION(BlueprintCallable, Category = ACF)
    EACFDirection GetRelativeTargetDirection(const AActor* targetActor) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ActivateDamage(const EDamageActivationType& damageActType, const TArray<FName>& traceChannels);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void DeactivateDamage(const EDamageActivationType& damageActType, const TArray<FName>& traceChannels);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SwitchMoveset(FGameplayTag moveType);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SwitchMovesetActions(FGameplayTag moveType);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACF)
    void SwitchOverlay(FGameplayTag overlayTag);
};
