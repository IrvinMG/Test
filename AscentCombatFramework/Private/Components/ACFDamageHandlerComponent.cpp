// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "Components/ACFDamageHandlerComponent.h"
#include "ARSStatisticsComponent.h"
#include "ARSTypes.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFTeamManagerComponent.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFDamageTypeCalculator.h"
#include "Game/ACFFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include <Components/MeshComponent.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>
#include <GameFramework/DamageType.h>
#include <Kismet/KismetSystemLibrary.h>
#include <PhysicsEngine/BodyInstance.h>
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
//#include "Engine/DamageEvents.h"

// Sets default values for this component's properties
UACFDamageHandlerComponent::UACFDamageHandlerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    bIsAlive = true;
    DamageCalculatorClass = UACFDamageTypeCalculator::StaticClass();
}

void UACFDamageHandlerComponent::InitializeDamageCollisions(ETeam combatTeam)
{
    UACFTeamManagerComponent* TeamManager = UACFFunctionLibrary::GetACFTeamManager(GetWorld());
    if (TeamManager) {
        AssignCollisionProfile(TeamManager->GetCollisionChannelByTeam(combatTeam, bUseBlockingCollisionChannel));
        inCombatTeam = combatTeam;
    } else {
        UE_LOG(LogTemp, Error, TEXT("Remember to add an ACFGameState to your World!!! -  UACFDamageHandlerComponent::InitializeDamageCollisions"));
    }
}

void UACFDamageHandlerComponent::AssignCollisionProfile(const TEnumAsByte<ECollisionChannel> channel)
{
    TArray<UActorComponent*> meshes;
    GetOwner()->GetComponents(UMeshComponent::StaticClass(), meshes);

    for (auto& mesh : meshes) {
        UMeshComponent* meshComp = Cast<UMeshComponent>(mesh);

        if (meshComp) {
            meshComp->SetCollisionObjectType(channel);
        }
    }
}

void UACFDamageHandlerComponent::BeginPlay()
{
    Super::BeginPlay();

    UARSStatisticsComponent* StatisticsComp = GetOwner()->FindComponentByClass<UARSStatisticsComponent>();
    if (StatisticsComp) {

        StatisticsComp->OnStatisiticReachesZero.AddDynamic(this, &UACFDamageHandlerComponent::HandleStatReachedZero);
    }
}

float UACFDamageHandlerComponent::TakeDamage(class AActor* damageReceiver, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (!damageReceiver) {
        return Damage;
    }

    FHitResult outDamage;
    FVector ShotDirection;
    DamageEvent.GetBestHitInfo(damageReceiver, DamageCauser, outDamage, ShotDirection);
    FACFDamageEvent outDamageEvent;
    ConstructDamageReceived(damageReceiver, Damage, EventInstigator, outDamage.Location, outDamage.Component.Get(), outDamage.BoneName, ShotDirection, DamageEvent.DamageTypeClass,
        DamageCauser);

    UARSStatisticsComponent* StatisticsComp = damageReceiver->FindComponentByClass<UARSStatisticsComponent>();

    if (StatisticsComp) {
        FStatisticValue statMod(UACFFunctionLibrary::GetHealthTag(), -LastDamageReceived.FinalDamage);
        StatisticsComp->ModifyStat(statMod);
    }
    ClientsReceiveDamage(LastDamageReceived);
    return LastDamageReceived.FinalDamage;
}

void UACFDamageHandlerComponent::Revive_Implementation()
{
    bIsAlive = true;
}

void UACFDamageHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UACFDamageHandlerComponent, bIsAlive);
}

void UACFDamageHandlerComponent::ConstructDamageReceived(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
    class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, TSubclassOf<UDamageType> DamageType,
    AActor* DamageCauser)
{

    FACFDamageEvent tempDamageEvent;
    tempDamageEvent.contextString = NAME_None;
    tempDamageEvent.FinalDamage = Damage;
    tempDamageEvent.DamageDealer = Cast<APawn>(DamageCauser);
    tempDamageEvent.hitDirection = ShotFromDirection;
    tempDamageEvent.hitResult.BoneName = BoneName;
    tempDamageEvent.hitResult.ImpactPoint = HitLocation;
    tempDamageEvent.hitResult.Location = HitLocation;
    tempDamageEvent.hitResult.HitObjectHandle = FActorInstanceHandle(DamagedActor);
    tempDamageEvent.DamageReceiver = DamagedActor;
    tempDamageEvent.DamageClass = DamageType;

    tempDamageEvent.DamageDirection = UACFFunctionLibrary::GetHitDirectionByHitResult(tempDamageEvent.DamageDealer, tempDamageEvent.hitResult);

    tempDamageEvent.HitResponseAction = FGameplayTag();

    AACFCharacter* acfReceiver = Cast<AACFCharacter>(DamagedActor);

    if (acfReceiver) {
        tempDamageEvent.DamageZone = acfReceiver->GetDamageZoneByBoneName(BoneName);
        FBodyInstance* bodyInstance = acfReceiver->GetMesh()->GetBodyInstance(BoneName);
        if (bodyInstance) {
            tempDamageEvent.PhysMaterial = bodyInstance->GetSimplePhysicalMaterial();
        }
    }

    if (DamageCalculatorClass) {
        if (!DamageCalculator) {
            DamageCalculator = NewObject<UACFDamageCalculation>(this, DamageCalculatorClass);
        }
        tempDamageEvent.HitResponseAction = DamageCalculator->EvaluateHitResponseAction(tempDamageEvent, HitResponseActions);
        tempDamageEvent.bIsCritical = DamageCalculator->IsCriticalDamage(tempDamageEvent);
        tempDamageEvent.FinalDamage = DamageCalculator->CalculateFinalDamage(tempDamageEvent);
    } else {
        ensure(false);
        UE_LOG(LogTemp, Error, TEXT("MISSING DAMAGE CALCULATOR CLASS -  UACFDamageHandlerComponent"));
    }

    LastDamageReceived = tempDamageEvent;
}

void UACFDamageHandlerComponent::HandleStatReachedZero(FGameplayTag stat)
{
    if (UACFFunctionLibrary::GetHealthTag() == stat) {

        if (GetOwner()->HasAuthority()) {
            UARSStatisticsComponent* StatisticsComp = GetOwner()->FindComponentByClass<UARSStatisticsComponent>();
            if (StatisticsComp) {
                StatisticsComp->StopRegeneration();
                if (LastDamageReceived.DamageDealer) {
                    UARSStatisticsComponent* dealerStatComp = LastDamageReceived.DamageDealer->FindComponentByClass<UARSStatisticsComponent>();
                    if (dealerStatComp) {
                        dealerStatComp->AddExp(StatisticsComp->GetExpOnDeath());
                    }
                }
            }
        }
        bIsAlive = false;
        OnOwnerDeath.Broadcast();
    }
}

void UACFDamageHandlerComponent::ClientsReceiveDamage_Implementation(const FACFDamageEvent& damageEvent)
{
    LastDamageReceived = damageEvent;

    OnDamageReceived.Broadcast(damageEvent);
}
