// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Actors/ACFCharacter.h"
#include "ACFCCFunctionLibrary.h"
#include "ACMCollisionManagerComponent.h"
#include "ARSStatisticsComponent.h"
#include "ATSBaseTargetComponent.h"
#include "ATSTargetingComponent.h"
#include "ActionsSystem/Public/ACFActionsFunctionLibrary.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFCharacterMovementComponent.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Components/ACFEffectsManagerComponent.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/ACFRagdollComponent.h"
#include "Components/ACFTeamManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Game/ACFDamageCalculation.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFGameInstance.h"
#include "Game/ACFGameMode.h"
#include "Game/ACFPlayerController.h"
#include "Items/ACFMeleeWeapon.h"
#include "Items/ACFRangedWeapon.h"
#include "Items/ACFWeapon.h"
#include "MotionWarpingComponent.h"
#include "Net/UnrealNetwork.h"
#include <Components/AudioComponent.h>
#include <Components/CapsuleComponent.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/DamageType.h>
#include <GenericTeamAgentInterface.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Perception/AIPerceptionStimuliSourceComponent.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <PhysicsEngine/BodyInstance.h>
#include "GameFramework/Actor.h"
#include "Engine/World.h"
// #include "Engine/DamageEvents.h"

// Sets default values
AACFCharacter::AACFCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) //.SetDefaultSubobjectClass<UACFCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to
    // improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    ActionsComp = CreateDefaultSubobject<UACFActionsManagerComponent>(TEXT("Actions Manager"));
    StatisticsComp = CreateDefaultSubobject<UARSStatisticsComponent>(TEXT("Statistic Component"));
    CollisionComp = CreateDefaultSubobject<UACMCollisionManagerComponent>(TEXT("Collisions Manager"));
    EquipmentComp = CreateDefaultSubobject<UACFEquipmentComponent>(TEXT("Equipment Component"));
    AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Perception Stimuli Component"));
    DamageHandlerComp = CreateDefaultSubobject<UACFDamageHandlerComponent>(TEXT("Damage Handler Component"));
    RagdollComp = CreateDefaultSubobject<UACFRagdollComponent>(TEXT("Ragdoll Component"));
    MotionWarpComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warp Component"));
    PrimaryActorTick.bStartWithTickEnabled = false;
    CombatTeam = ETeam::ETeam1;

    AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
    AudioComp->SetupAttachment(GetCapsuleComponent());
    AudioComp->SetRelativeLocation(FVector::ZeroVector);
    AudioComp->bAllowSpatialization = true;
    AudioComp->bOverrideAttenuation = false;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    CharacterName = FText::FromString("Dude");

    bIsImmortal = false;
    BoneNameToDamageZoneMap.Add("head", EDamageZone::EHighDamage);
}

void AACFCharacter::PostInitProperties()
{
    Super::PostInitProperties();
    EffetsComp = FindComponentByClass<UACFEffectsManagerComponent>();
    LocomotionComp = Cast<UACFCharacterMovementComponent>(GetCharacterMovement());

    if (!LocomotionComp) {
        UE_LOG(LogTemp, Warning, TEXT("Your Character Movement component MUST BE an ACFCharacterMovementComponent - AACFCharacter::PostInitProperties"));
    }
}

void AACFCharacter::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    /* ensureAlways(!IsAlive());*/
}

void AACFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AACFCharacter, CombatTeam);
    DOREPLIFETIME(AACFCharacter, CombatType);
    DOREPLIFETIME_CONDITION(AACFCharacter, ReplicatedAcceleration, COND_SimulatedOnly);
}

void AACFCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);

    if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement()) {
        // Compress Acceleration: XY components as direction + magnitude, Z component as direct value
        const double MaxAccel = MovementComponent->MaxAcceleration;
        const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
        double AccelXYRadians, AccelXYMagnitude;
        FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

        ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0); // [0, 2PI] -> [0, 255]
        ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0); // [0, MaxAccel] -> [0, 255]
        ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0); // [-MaxAccel, MaxAccel] -> [-127, 127]
    }
}

void AACFCharacter::OnRep_ReplicatedAcceleration()
{
    if (LocomotionComp) {
        // Decompress Acceleration
        const double MaxAccel = LocomotionComp->MaxAcceleration;
        const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
        const double AccelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0; // [0, 255] -> [0, 2PI]

        FVector UnpackedAcceleration(FVector::ZeroVector);
        FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
        UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

        LocomotionComp->SetReplicatedAcceleration(UnpackedAcceleration);
    }
}

void AACFCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

// Called when the game starts or when spawned
void AACFCharacter::BeginPlay()
{
   
    EffetsComp = FindComponentByClass<UACFEffectsManagerComponent>();

    InitializeCharacter();
    Super::BeginPlay();
}

void AACFCharacter::InitializeCharacter()
{
    if (HasAuthority() && !bInitialized) {
        if (ActionsComp) {
            ActionsComp->SetMovesetActions(
                UACFActionsFunctionLibrary::GetMovesetActionsTagRoot());
        }
        if (StatisticsComp) {
            StatisticsComp->InitializeAttributeSet();
        }

        if (DamageHandlerComp) {
            DamageHandlerComp->OnOwnerDeath.AddDynamic(
                this, &AACFCharacter::HandleCharacterDeath);
        }

        if (EquipmentComp) {
            EquipmentComp->InitializeInventoryAndEquipment(GetMainMesh());
        }
    }

    if (EquipmentComp) {
        EquipmentComp->OnEquipmentChanged.AddDynamic(
            this, &AACFCharacter::HandleEquipmentChanged);
    }

    if (DamageHandlerComp) {
        DamageHandlerComp->OnDamageReceived.AddDynamic(this, &AACFCharacter::HandleDamageReceived);
    }

    if (CollisionComp) {
        CollisionComp->SetupCollisionManager(GetMesh());
    }
    bInitialized = true;
    OnCharacterFullyInitialized.Broadcast();
}

void AACFCharacter::HandleEquipmentChanged(const FEquipment& equipment)
{
    FGameplayTag movesetTag, overlayTag, actionsTag;

    if (EquipmentComp) {
        actionsTag = EquipmentComp->GetCurrentDesiredMovesetActionTag();
        overlayTag = EquipmentComp->GetCurrentDesiredOverlayTag();
        movesetTag = EquipmentComp->GetCurrentDesiredMovesetTag();

        if (movesetTag == FGameplayTag()) {
            movesetTag = UACFCCFunctionLibrary::GetMovesetTypeTagRoot();
        }
        if (overlayTag == FGameplayTag()) {
            overlayTag = UACFCCFunctionLibrary::GetAnimationOverlayTagRoot();
        }

        if (!equipment.MainWeapon) {
            CombatType = ECombatType::EUnarmed;
        } else if (equipment.MainWeapon->IsA<AACFMeleeWeapon>()) {
            CombatType = ECombatType::EMelee;
        } else if (equipment.MainWeapon->IsA<AACFRangedWeapon>()) {
            CombatType = ECombatType::ERanged;
        }
    } else {
        movesetTag = UACFCCFunctionLibrary::GetMovesetTypeTagRoot();
        overlayTag = UACFCCFunctionLibrary::GetAnimationOverlayTagRoot();
        CombatType = ECombatType::EUnarmed;
    }

    if (movesetTag == FGameplayTag()) {
        movesetTag = UACFCCFunctionLibrary::GetMovesetTypeTagRoot();
    }
    if (overlayTag == FGameplayTag()) {
        overlayTag = UACFCCFunctionLibrary::GetAnimationOverlayTagRoot();
    }

    UACFAnimInstance* acfAnimInst = GetACFAnimInstance();
    if (acfAnimInst) {
        if (movesetTag != FGameplayTag() && movesetTag != acfAnimInst->GetCurrentMovesetTag()) {
            acfAnimInst->SetMoveset(movesetTag);
        }
        if (overlayTag != acfAnimInst->GetCurrentOverlay()) {
            acfAnimInst->SetAnimationOverlay(overlayTag);
        }
    }

    if (ActionsComp) {
        ActionsComp->SetMovesetActions(actionsTag);
    }

    OnCombatTypeChanged.Broadcast(CombatType);
}

void AACFCharacter::AssignTeam(ETeam team)
{
    CombatTeam = team;

    SetGenericTeamId(uint8(CombatTeam));

    if (DamageHandlerComp) {
        DamageHandlerComp->InitializeDamageCollisions(CombatTeam);
    }
    SetupCollisionManager();

    OnTeamChanged.Broadcast(CombatTeam);
}

void AACFCharacter::OnActorLoaded_Implementation()
{
    if (!GetController()) {
        SpawnDefaultController();
    }
}

void AACFCharacter::OnActorSaved_Implementation() { }

ETeam AACFCharacter::GetCombatTeam() const
{
    return CombatTeam;
}

void AACFCharacter::HandleDamageReceived(const FACFDamageEvent& damageReceived)
{
    OnDamageReceived.Broadcast(damageReceived);
}

void AACFCharacter::SetupCollisionManager()
{
    if (CollisionComp) {
        CollisionComp->OnCollisionDetected.RemoveAll(this);
        CollisionComp->SetupCollisionManager(GetMesh());
        CollisionComp->SetActorOwner(this);
        CollisionComp->OnCollisionDetected.AddDynamic(
            this, &AACFCharacter::HandleAttackHit);
        CollisionComp->SetCollisionChannels(GetEnemiesCollisionChannel());
        CollisionComp->AddActorToIgnore(this);
    }
}

void AACFCharacter::Internal_SwitchMovetype(const FGameplayTag& moveType)
{
    if (GetACFAnimInstance()) {
        GetACFAnimInstance()->SetMoveset(moveType);
    }
    OnMovesetChanged.Broadcast(moveType);
}

void AACFCharacter::SwitchOverlay_Implementation(FGameplayTag overlayTag)
{
    ClientsSwitchOverlay(overlayTag);
}

void AACFCharacter::ClientsSwitchOverlay_Implementation(const FGameplayTag& overlayTag)
{
    if (GetACFAnimInstance()) {
        GetACFAnimInstance()->SetAnimationOverlay(overlayTag);
    }
}

void AACFCharacter::ClientsSwitchMovetype_Implementation(const FGameplayTag& moveType)
{
    Internal_SwitchMovetype(moveType);
}

void AACFCharacter::SwitchMovesetActions_Implementation(FGameplayTag moveType)
{
    if (ActionsComp) {
        ActionsComp->SetMovesetActions(moveType);
    }
}

void AACFCharacter::SwitchMoveset_Implementation(FGameplayTag moveType)
{
    ClientsSwitchMovetype(moveType);
}

ECollisionChannel AACFCharacter::GetCollisionChannel() const
{
    const UACFTeamManagerComponent* teamManager = UACFFunctionLibrary::GetACFTeamManager(GetWorld());
    if (teamManager) {
        return teamManager->GetCollisionChannelByTeam(CombatTeam);
    }
    UE_LOG(LogTemp, Error, TEXT("Missing ACFTeamManagerComponent - AACFCharacter::GetEnemiesCollisionChannel"));
    return ECollisionChannel::ECC_MAX;
}

TArray<TEnumAsByte<ECollisionChannel>> AACFCharacter::GetEnemiesCollisionChannel() const
{
    TArray<TEnumAsByte<ECollisionChannel>> channels;

    UACFTeamManagerComponent* teamManager = UACFFunctionLibrary::GetACFTeamManager(GetWorld());

    if (!teamManager) {
        UE_LOG(LogTemp, Error, TEXT("Missing ACFTeamManagerComponent - AACFCharacter::GetEnemiesCollisionChannel"));
        return channels;
    }

    channels = teamManager->GetEnemiesCollisionChannels(CombatTeam);
    return channels;
}

bool AACFCharacter::CanBeRanged() const
{
    return EquipmentComp->HasOnBodyAnyWeaponOfType(
        AACFRangedWeapon::StaticClass());
}

// Called to bind functionality to input
void AACFCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AACFCharacter::HandleCharacterDeath()
{

    switch (DeathType) {
    case EDeathType::EDeathAction:
        ForceAction(UACFFunctionLibrary::GetDefaultDeathState());
        break;
    case EDeathType::EGoRagdoll:
        RagdollComp->GoRagdollFromDamage(GetLastDamageInfo());
        break;
    default:
        break;
    }
    if (EquipmentComp) {
        EquipmentComp->OnEntityOwnerDeath();
    }
    if (bAutoDestroyOnDeath) {
        SetLifeSpan(DestroyTimeOnDeath);
    }
    OnDeath.Broadcast(this);
    ClientsOnCharacterDeath();
}

void AACFCharacter::ClientsOnCharacterDeath_Implementation()
{
    if (DeathType == EDeathType::EGoRagdoll) {
        RagdollComp->GoRagdollFromDamage(GetLastDamageInfo());
    }
    if (bDisableCapsuleOnDeath && GetCapsuleComponent()) {
        GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
        GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    }
    OnCharacterDeath();
}

AActor* AACFCharacter::GetTarget() const
{
    const AController* controller = GetController();
    if (controller) {
        const UATSBaseTargetComponent* targetContr = controller->FindComponentByClass<UATSBaseTargetComponent>();
        if (targetContr) {
            return targetContr->GetCurrentTarget();
        }
    }
    return nullptr;
}

class UATSBaseTargetComponent* AACFCharacter::GetTargetingComponent() const
{
    const AController* controller = GetController();
    if (controller) {
        return controller->FindComponentByClass<UATSBaseTargetComponent>();
    }
    return nullptr;
}

bool AACFCharacter::IsAlive() const
{
    return GetDamageHandlerComponent()->GetIsAlive();
}

FGameplayTag AACFCharacter::GetCurrentMoveset() const
{
    return GetACFAnimInstance()->GetCurrentMovesetTag();
}

UACFAnimInstance* AACFCharacter::GetACFAnimInstance() const
{
    return Cast<UACFAnimInstance>(GetMesh()->GetAnimInstance());
}

void AACFCharacter::MulticastPlayAnimMontage_Implementation(
    class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/,
    FName StartSectionName /*= NAME_None*/, float rootMotionScale)
{
    SetAnimRootMotionTranslationScale(rootMotionScale);
    PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

bool AACFCharacter::MulticastPlayAnimMontage_Validate(
    class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /* = NAME_None*/, float rootMotionScale)
{
    return true;
}

void AACFCharacter::ServerSendPlayMontage_Implementation(
    class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/,
    FName StartSectionName /*= NAME_None*/, float rootMotionScale)
{
    if (AnimMontage) {
        SetAnimRootMotionTranslationScale(rootMotionScale);
        MulticastPlayAnimMontage(AnimMontage, InPlayRate, StartSectionName, rootMotionScale);
    } else {
        UE_LOG(LogTemp, Log, TEXT("Invalid Montage"));
    }
}

bool AACFCharacter::ServerSendPlayMontage_Validate(
    class UAnimMontage* AnimMontage, float InPlayRate = 1.f,
    FName StartSectionName = NAME_None, float rootMotionScale = 1.f)
{
    return true;
}

void AACFCharacter::ForceAction(FGameplayTag Action)
{
    TriggerAction(Action, EActionPriority::EHighest);
}

void AACFCharacter::TriggerAction(FGameplayTag Action, EActionPriority Priority, bool canBeStored /*= false*/, const FString& contextString /*= ""*/)
{
    if (ActionsComp) {
        ActionsComp->TriggerAction(Action, Priority, canBeStored, contextString);
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Attempting to trigger action (%s) without an action manager component"), *Action.ToString());
}

void AACFCharacter::ForceActionByName(FName ActionName)
{
    if (ActionsComp) {
        ActionsComp->TriggerActionByName(ActionName, EActionPriority::EHighest);
    }
}

FGameplayTag AACFCharacter::GetCurrentActionState() const
{

    if (ActionsComp) {
        return ActionsComp->GetCurrentActionTag();
    }

    return FGameplayTag();
}

EDamageZone AACFCharacter::GetDamageZoneByBoneName(FName BoneName) const
{
    const EDamageZone* zone = BoneNameToDamageZoneMap.Find(BoneName);
    if (zone) {
        return *zone;
    }
    return EDamageZone::ENormal;
}

float AACFCharacter::GetWalkSpeed() const
{
    if (LocomotionComp)
        return LocomotionComp->GetCharacterMaxSpeedByState(ELocomotionState::EWalk);

    return 0.f;
}

float AACFCharacter::GetJogSpeed() const
{
    if (LocomotionComp)
        return LocomotionComp->GetCharacterMaxSpeedByState(ELocomotionState::EJog);

    return 0.f;
}

float AACFCharacter::GetSprintSpeed() const
{
    if (LocomotionComp)
        return LocomotionComp->GetCharacterMaxSpeedByState(
            ELocomotionState::ESprint);

    return 0.f;
}

float AACFCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (!IsAlive() || bIsImmortal)
        return 0.f;

    const float damageReceived = DamageHandlerComp->TakeDamage(this, Damage, DamageEvent, EventInstigator, DamageCauser);

    if (GetLastDamageInfo().HitResponseAction != FGameplayTag() && IsAlive()) {
        ForceAction(GetLastDamageInfo().HitResponseAction);
    }

    const AACFCharacter* dealer = Cast<AACFCharacter>(GetLastDamageInfo().DamageDealer);
    if (dealer) {
        dealer->OnDamageInflicted.Broadcast(this);
    }
    return damageReceived;
}

void AACFCharacter::Crouch(bool bClientSimulation /*= false*/)
{

    Super::Crouch(bClientSimulation);
    OnCrouchStateChanged.Broadcast(GetCharacterMovement()->IsCrouching());
}

void AACFCharacter::UnCrouch(bool bClientSimulation)
{
    Super::UnCrouch(bClientSimulation);
    OnCrouchStateChanged.Broadcast(GetCharacterMovement()->IsCrouching());
}

void AACFCharacter::OnMovementModeChanged(EMovementMode prevMovementMde,
    uint8 PreviousCustomMode /*= 0*/)
{
    Super::OnMovementModeChanged(prevMovementMde, PreviousCustomMode);

    if (GetMovementComponent()->IsFalling()) {
        ZFalling = GetActorLocation().Z;
    }
}

void AACFCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (HasAuthority()) {
        if (ZFalling == -1.f)
            return;

        const float totalFallinDistance = ZFalling - GetActorLocation().Z;
        ZFalling = -1.f;
        const float fallDamage = GetFallDamageFromDistance(totalFallinDistance);
        if (fallDamage > 1.f) {
            FDamageEvent damageFall;
            damageFall.DamageTypeClass = UFallDamageType::StaticClass();
            TakeDamage(fallDamage, damageFall, GetController(), this);
        }

        if (bTriggerActionOnLand && totalFallinDistance > FallHeightToTriggerAction) {
            ForceAction(ActionsToTriggerOnLand);
        }
    }
}

void AACFCharacter::OnCharacterDeath_Implementation()
{
}

USkeletalMeshComponent* AACFCharacter::GetMainMesh_Implementation() const
{
    return GetMesh();
}

float AACFCharacter::GetFallDamageFromDistance(float fallDistance)
{
    const float fallDamageDist = fallDistance - FallDamageDistanceThreshold;
    if (FallDamageByFallDistance && fallDamageDist > 0) {
        return FallDamageByFallDistance->GetFloatValue(fallDamageDist);
    }
    return 0.f;
}

void AACFCharacter::ActivateDamage(const EDamageActivationType& damageActType,
    const TArray<FName>& traceChannels)
{
    if (damageActType == EDamageActivationType::EPhysical && CollisionComp) {
        if (!CollisionComp->IsRegistered()) {
            CollisionComp->RegisterComponent();
        }

        if (!traceChannels.IsValidIndex(0)) {
            CollisionComp->StartAllTraces();
        }

        else {
            for (const auto& channel : traceChannels) {
                CollisionComp->StartSingleTrace(channel);
            }
        }
    } else if (EquipmentComp) {
        switch (damageActType) {
        case EDamageActivationType::ERight:
            EquipmentComp->SetDamageActivation(true, traceChannels, false);
            break;
        case EDamageActivationType::ELeft:
            EquipmentComp->SetDamageActivation(true, traceChannels, true);
            break;
        case EDamageActivationType::EBoth:
            EquipmentComp->SetDamageActivation(true, traceChannels, true);
            EquipmentComp->SetDamageActivation(true, traceChannels, false);
            break;
        }
    }
}

void AACFCharacter::DeactivateDamage(const EDamageActivationType& damageActType,
    const TArray<FName>& traceChannels)
{
    if (damageActType == EDamageActivationType::EPhysical && CollisionComp) {
        if (!traceChannels.IsValidIndex(0)) {
            CollisionComp->StopAllTraces();

        } else {
            for (const auto& channel : traceChannels)
                CollisionComp->StopSingleTrace(channel);
        }

    } else if (EquipmentComp) {
        switch (damageActType) {
        case EDamageActivationType::ERight:
            EquipmentComp->SetDamageActivation(false, traceChannels, false);
            break;
        case EDamageActivationType::ELeft:
            EquipmentComp->SetDamageActivation(false, traceChannels, true);
            break;
        case EDamageActivationType::EBoth:
            EquipmentComp->SetDamageActivation(false, traceChannels, true);
            EquipmentComp->SetDamageActivation(false, traceChannels, false);
            break;
        }
    }
}

bool AACFCharacter::IsMyEnemy(AACFCharacter* target) const
{
    if (!target)
        return false;

    if (UACFFunctionLibrary::GetBattleType(GetWorld()) == EBattleType::EEveryoneAgainstEveryone)
        return true;
    else {
        return UACFFunctionLibrary::AreEnemyTeams(GetWorld(), CombatTeam,
            target->GetCombatTeam());
    }
}

float AACFCharacter::GetBaseDamage_Implementation() { return 1.f; }

void AACFCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AACFCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
    /*Super::SetGenericTeamId(InTeamID);*/
    CombatTeam = (ETeam)InTeamID.GetId();
}

FGenericTeamId AACFCharacter::GetGenericTeamId() const
{
    return uint8(CombatTeam);
}

void AACFCharacter::TryJump()
{
    if (ActionsComp->IsInActionState(UACFActionsFunctionLibrary::GetDefaultActionsState())) {
        Jump();
    }
}

void AACFCharacter::HandleAttackHit(const FHitResult& HitResult)
{
    OnDamageInflicted.Broadcast(Cast<AACFCharacter>(HitResult.GetActor()));
}

void AACFCharacter::KillCharacter_Implementation()
{
    const FGameplayTag healthTag = UACFFunctionLibrary::GetHealthTag();
    const float damageToApply = StatisticsComp->GetCurrentValueForStatitstic(healthTag);

    StatisticsComp->ModifyStatistic(healthTag, -damageToApply);
}

void AACFCharacter::ReviveCharacter_Implementation(float normalizedHealthToGrant)
{
    const FGameplayTag healthTag = UACFFunctionLibrary::GetHealthTag();
    const float healthToApply = GetStatisticsComponent()->GetMaxValueForStatitstic(healthTag);
    GetDamageHandlerComponent()->Revive();
    GetStatisticsComponent()->ModifyStatistic(healthTag, healthToApply * normalizedHealthToGrant);
}

EACFDirection AACFCharacter::GetRelativeTargetDirection(const AActor* targetActor) const
{
    if (!targetActor)
        return EACFDirection::Front;

    const float dotProduct = GetDotProductTo(targetActor);

    if (dotProduct >= 0.5f)
        return EACFDirection::Front;

    else if (dotProduct <= -0.5f)
        return EACFDirection::Back;

    FVector dirVectorToReceiver = UKismetMathLibrary::GetDirectionUnitVector(
        GetActorLocation(), targetActor->GetActorLocation());

    float dotProductFromRight = UKismetMathLibrary::Dot_VectorVector(
        GetActorRightVector(), dirVectorToReceiver);

    if (dotProductFromRight >= 0.f)
        return EACFDirection::Right;
    else
        return EACFDirection::Left;
}

float AACFCharacter::GetCurrentMaxSpeed() const
{
    return GetCharacterMovement()->MaxWalkSpeed;
}

FACFDamageEvent AACFCharacter::GetLastDamageInfo() const
{
    return DamageHandlerComp->GetLastDamageInfo();
}

void AACFCharacter::SetBase(UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor)
{
    if (NewBase) {
        AActor* BaseOwner = NewBase->GetOwner();
        // LoadClass to not depend on the voxel module
        static UClass* VoxelWorldClass = LoadClass<UObject>(nullptr, TEXT("/Script/Voxel.VoxelWorld"));
        if (VoxelWorldClass && BaseOwner && BaseOwner->IsA(VoxelWorldClass)) {
            NewBase = Cast<UPrimitiveComponent>(BaseOwner->GetRootComponent());
            ensure(NewBase);
        }
    }
    Super::SetBase(NewBase, BoneName, bNotifyActor);
}
