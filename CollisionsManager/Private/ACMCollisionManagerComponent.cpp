// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ACMCollisionManagerComponent.h"
#include <Components/MeshComponent.h>
#include <WorldCollision.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Templates/Function.h>
#include <Engine/EngineTypes.h>
#include "ACMCollisionsMasterComponent.h"
#include <GameFramework/GameMode.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameModeBase.h>
#include <Particles/ParticleSystemComponent.h>
#include <Sound/SoundBase.h>
#include <Components/SceneComponent.h>
#include <Sound/SoundCue.h>
#include "Components/ActorComponent.h"
#include "DrawDebugHelpers.h"
#include "ACMCollisionsFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "NiagaraFunctionLibrary.h"
#include "ACMTypes.h"
#include "NiagaraComponent.h"
#include "Niagara/Classes/NiagaraSystem.h"

// Sets default values for this component's properties
UACMCollisionManagerComponent::UACMCollisionManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetComponentTickEnabled(false);
}

void UACMCollisionManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    SetComponentTickEnabled(false);
    SetStarted(false);
}

void UACMCollisionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority()) {
		StopCurrentAreaDamage();
		StopAllTraces();
	}
    Super::EndPlay(EndPlayReason);
}

void UACMCollisionManagerComponent::UpdateCollisions()
{
    if (damageMesh) {
        DisplayDebugTraces();

        if (pendingDelete.IsValidIndex(0)) {

            for (const auto toDelete : pendingDelete) {
                if (activatedTraces.Contains(toDelete)) {
                    activatedTraces.Remove(toDelete);
                }
                alreadyHitActors.Remove(toDelete);
            }
            pendingDelete.Empty();
        }
        if (activatedTraces.Num() == 0) {
            SetStarted(false);
            return;
        }
        if (CollisionChannels.IsValidIndex(0)) {
            for (TPair<FName, FTraceInfo>& currentTrace : activatedTraces) {
                if (damageMesh->DoesSocketExist(currentTrace.Value.StartSocket) && damageMesh->DoesSocketExist(currentTrace.Value.EndSocket)) {
                    FHitResult hitRes;

                    const FVector StartPos = damageMesh->GetSocketLocation(currentTrace.Value.StartSocket);
                    const FVector EndPos = damageMesh->GetSocketLocation(currentTrace.Value.EndSocket);
                    const FRotator orientation = GetLineRotation(StartPos, EndPos);
                    FCollisionQueryParams Params;

                    if (IgnoredActors.Num() > 0) {
                        Params.AddIgnoredActors(IgnoredActors);
                    }

                    if (bIgnoreOwner) {
                        Params.AddIgnoredActor(GetActorOwner());
                    }

					Params.bReturnPhysicalMaterial = true;
					Params.bTraceComplex = true;

					FCollisionObjectQueryParams ObjectParams;
					for (const TEnumAsByte<ECollisionChannel>& channel : CollisionChannels) {
						ObjectParams.AddObjectTypesToQuery(channel);
					}


                    if (!bAllowMultipleHitsPerSwing) {
                        FHitActors* hitResact = alreadyHitActors.Find(currentTrace.Key);
                        if (hitResact && hitResact->AlreadyHitActors.Num() > 0) {
                            Params.AddIgnoredActors(hitResact->AlreadyHitActors);
                        }
                    }
              
                    if (ObjectParams.IsValid() == false) {
                        UE_LOG(LogTemp, Warning, TEXT("Invalid Collision Channel"));
                        return;
                    }

                    UWorld* world = GetWorld();
                    if (world) {
                        bool bHit = world->SweepSingleByObjectType(
                            hitRes, StartPos, EndPos, orientation.Quaternion(), ObjectParams, FCollisionShape::MakeSphere(currentTrace.Value.Radius), Params);

                        if (!bHit && currentTrace.Value.bCrossframeAccuracy && !currentTrace.Value.bIsFirstFrame) {
                            const FRotator oldOrient = GetLineRotation(StartPos, EndPos);
							bHit = world->SweepSingleByObjectType(
								hitRes, StartPos, currentTrace.Value.oldEndSocketPos, oldOrient.Quaternion(), ObjectParams, FCollisionShape::MakeSphere(currentTrace.Value.Radius), Params);

                        }
                        if (bHit) {
                            OnCollisionDetected.Broadcast(hitRes);
                            if (!bAllowMultipleHitsPerSwing) {
                                FHitActors* hitResact = alreadyHitActors.Find(currentTrace.Key);
                                if (hitResact) {
                                    hitResact->AlreadyHitActors.Add(hitRes.GetActor());
                                } else {
                                    FHitActors newHit;
                                    newHit.AlreadyHitActors.Add(hitRes.GetActor());
                                    alreadyHitActors.Add(currentTrace.Key, newHit);
                                }
                            }
                            ApplyDamage(hitRes, currentTrace.Value);
                     
                        }
                        currentTrace.Value.bIsFirstFrame = false;
                        currentTrace.Value.oldEndSocketPos = EndPos;
                    }
                } else {
                    UE_LOG(LogTemp, Warning, TEXT("Invalid Socket Names!!"));
                }
            }
        } else {
            SetStarted(false);
        }
    }
}

void UACMCollisionManagerComponent::SetStarted(bool inStarted)
{
    bIsStarted = inStarted;
    AGameModeBase* gameMode = UGameplayStatics::GetGameMode(this);
    if (gameMode) {
        UACMCollisionsMasterComponent* collisionMaster = gameMode->FindComponentByClass<UACMCollisionsMasterComponent>();
        if (collisionMaster) {
            if (ShowDebugInfo == EDebugType::EAlwaysShowDebug || bIsStarted) {
                collisionMaster->AddComponent(this);
            } else {
                collisionMaster->RemoveComponent(this);
            }
        } else {
            UE_LOG(LogTemp, Error, TEXT("Add Collisions Master o your Game Mode!"));
        }
    }
}

FRotator UACMCollisionManagerComponent::GetLineRotation(FVector start, FVector end)
{
    const FVector diff = end - start;
    return diff.Rotation();
}

void UACMCollisionManagerComponent::SetupCollisionManager(class UMeshComponent* inDamageMesh)
{
    damageMesh = inDamageMesh;

    if (!damageMesh) {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Damage mesh!!"));
        return;
    }

    for (const auto& trace : DamageTraces) {
        UParticleSystemComponent* ParticleSystemComp = NewObject<UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass());
        ParticleSystemComp->SetupAttachment(damageMesh);
        ParticleSystemComp->SetRelativeLocation(FVector::ZeroVector);
        ParticleSystemComponents.Add(trace.Key, ParticleSystemComp);
        ParticleSystemComp->RegisterComponent();
    }
}


void UACMCollisionManagerComponent::StartAreaDamage_Implementation(const FVector& damageCenter, float damageRadius, float damageInterval /*= 1.f*/)
{
	UWorld* world = GetWorld();
	if (world) {
		PerformAreaDamage_Single(damageCenter, damageRadius);
 		world->GetTimerManager().SetTimer(
 			AreaDamageTimer, this, &UACMCollisionManagerComponent::HandleAreaDamageLooping, damageInterval,true);
	}
}

void UACMCollisionManagerComponent::StopCurrentAreaDamage_Implementation()
{
    if (currentAreaDamage.bIsActive) {
        currentAreaDamage.bIsActive = false;
		UWorld* world = GetWorld();
		world->GetTimerManager().ClearTimer(currentAreaDamage.AreaLoopTimer);
    }
}

void UACMCollisionManagerComponent::PerformAreaDamage_Single_Implementation(const FVector& damageCenter, float damageRadius)
{
    TArray<FHitResult> outHits;
    PerformAreaDamage_Single_Local(damageCenter, damageRadius, outHits);
}

void UACMCollisionManagerComponent::PerformAreaDamage_Single_Local(const FVector& damageCenter, float damageRadius, TArray<FHitResult>& outHits)
{
    FCollisionQueryParams Params;
    if (IgnoredActors.Num() > 0) {
        Params.AddIgnoredActors(IgnoredActors);
    }

    if (bIgnoreOwner) {
        Params.AddIgnoredActor(GetActorOwner());
    }

    UWorld* world = GetWorld();
    outHits.Empty();
    if (world) {
        for (const TEnumAsByte<ECollisionChannel>& channel : CollisionChannels) {
            TArray<FHitResult> outResults;
            const bool bHit = world->SweepMultiByChannel(outResults, damageCenter, damageCenter + FVector(1.f), FQuat::Identity, channel, FCollisionShape::MakeSphere(damageRadius), Params);
            if (bHit) {
                outHits.Append(outResults);
            }
        }
        for (const auto& hit : outHits) {
            ApplyDamage(hit, AreaDamageTraceInfo);
        }
        if ((uint8)ShowDebugInfo > 0) {
            ShowDebugTrace(damageCenter, damageCenter + FVector(1.f), damageRadius, EDrawDebugTrace::ForDuration, 3.f, FColor::Red);
        }
    }
}

void UACMCollisionManagerComponent::PerformAreaDamageForDuration_Implementation(const FVector& damageCenter, float damageRadius, float duration, float damageInterval /*= 1.f*/)
{
	UWorld* world = GetWorld();
	if (world) {
        StartAreaDamage(damageCenter, damageRadius, damageInterval);
        
 		world->GetTimerManager().SetTimer(
 			AreaDamageTimer, this, &UACMCollisionManagerComponent::HandleAreaDamageFinished, 
 			duration, false);
	}
}

void UACMCollisionManagerComponent::AddActorToIgnore(class AActor* ignoredActor) 
{ 
    IgnoredActors.AddUnique(ignoredActor); 
}

void UACMCollisionManagerComponent::AddCollisionChannel(TEnumAsByte<ECollisionChannel> inTraceChannel) 
{ 
    CollisionChannels.AddUnique(inTraceChannel); 
}

void UACMCollisionManagerComponent::SetCollisionChannels(TArray<TEnumAsByte<ECollisionChannel>> inTraceChannels)
{
    ClearCollisionChannels();

    for (const TEnumAsByte<ECollisionChannel>& chan : inTraceChannels) {
        AddCollisionChannel(chan);
    }
}

void UACMCollisionManagerComponent::ClearCollisionChannels() 
{ 
    CollisionChannels.Empty(); 
}

void UACMCollisionManagerComponent::PerformSwipeTraceShot_Implementation(const FVector& start, const FVector& end, float radius = 0.f)
{
    FHitResult outHit;
    PerformSwipeTraceShot_Local(start, end, radius, outHit );
}

void UACMCollisionManagerComponent::PerformSwipeTraceShot_Local(const FVector& start, const FVector& end, float radius /*= 0.f*/, FHitResult& outHit)
{
    if (actorOwner) {

        EDrawDebugTrace::Type debugType;
        switch (ShowDebugInfo) {
        case EDebugType::EAlwaysShowDebug:
        case EDebugType::EShowInfoDuringSwing:
            DrawDebugLine(GetWorld(), start, end, FColor::Red, true, 5.f, 1, 1.f);
            break;
        case EDebugType::EDontShowDebugInfos:
            debugType = EDrawDebugTrace::Type::None;
            break;
        default:
            debugType = EDrawDebugTrace::Type::None;
            break;
        }

        FCollisionQueryParams Params;
        if (IgnoredActors.Num() > 0) {
            Params.AddIgnoredActors(IgnoredActors);
        }

        if (bIgnoreOwner) {
            Params.AddIgnoredActor(GetActorOwner());
        }

        Params.bReturnPhysicalMaterial = true;
        Params.bTraceComplex = true;
        FCollisionObjectQueryParams ObjectParams;
        for (const TEnumAsByte<ECollisionChannel>& channel : CollisionChannels) {
            ObjectParams.AddObjectTypesToQuery(channel);
        }

        UWorld* world = GetWorld();
        if (world) {
            FHitResult outResult;
            const FRotator orientation = GetLineRotation(start, end);

            const bool bHit = world->SweepSingleByObjectType(
                outResult, start, end, orientation.Quaternion(), ObjectParams, FCollisionShape::MakeSphere(radius), Params);

            if (bHit) {
                ApplyDamage(outResult, SwipeTraceInfo);
                outHit = outResult;
                OnCollisionDetected.Broadcast(outResult);
            }

            switch (ShowDebugInfo) {
            case EDebugType::EAlwaysShowDebug:
            case EDebugType::EShowInfoDuringSwing:
                ShowDebugTrace(start, end, radius, EDrawDebugTrace::ForDuration, 3.f, FColor::Red);
                break;
            case EDebugType::EDontShowDebugInfos:
                break;
            default:
                break;
            }
        }
    }
}

void UACMCollisionManagerComponent::StartAllTraces_Implementation()
{
    activatedTraces.Empty();
    pendingDelete.Empty();

    for (const auto& damage : DamageTraces) {
        StartSingleTrace(damage.Key);
    }
}

void UACMCollisionManagerComponent::StopAllTraces_Implementation()
{
    pendingDelete.Empty();
    for (const auto& trace : activatedTraces) {
        StopSingleTrace(trace.Key);
    }
}

void UACMCollisionManagerComponent::StartSingleTrace_Implementation(const FName& Name)
{
    FTraceInfo* outTrace = DamageTraces.Find(Name);
    if (outTrace) {
        if (pendingDelete.Contains(Name)) {
            pendingDelete.Remove(Name);
        }
        outTrace->bIsFirstFrame = true;
        activatedTraces.Add(Name, *outTrace);
        PlayTrails(Name);
        SetStarted(true);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Trace Name!!"));
    }
}

void UACMCollisionManagerComponent::StopSingleTrace_Implementation(const FName& Name)
{
    if (activatedTraces.Contains(Name)) {
        StopTrails(Name);
        pendingDelete.AddUnique(Name);

        FHitActors* alreadyHit = alreadyHitActors.Find(Name);
        if (alreadyHit) {
            alreadyHit->AlreadyHitActors.Empty();
        }
    }
}

void UACMCollisionManagerComponent::DisplayDebugTraces()
{
    TMap<FName, FTraceInfo> _sphere;

    FLinearColor DebugColor;
    switch (ShowDebugInfo) {
    case EDebugType::EAlwaysShowDebug:
        _sphere = DamageTraces;
        if (bIsStarted)
            DebugColor = DebugActiveColor;
        else
            DebugColor = DebugInactiveColor;
        break;
    case EDebugType::EShowInfoDuringSwing:
        if (bIsStarted) {
            _sphere = activatedTraces;

            DebugColor = DebugActiveColor;
        } else
            return;
        break;
    case EDebugType::EDontShowDebugInfos:
        return;
    default:
        return;
    }

  
    for (const TPair<FName, FTraceInfo>& box : _sphere) {
        if (damageMesh->DoesSocketExist(box.Value.StartSocket) && damageMesh->DoesSocketExist(box.Value.EndSocket)) {
           

            const FVector StartPos = damageMesh->GetSocketLocation(box.Value.StartSocket);
            const FVector EndPos = damageMesh->GetSocketLocation(box.Value.EndSocket);
            const float radius = box.Value.Radius;

            ShowDebugTrace(StartPos, EndPos, radius, EDrawDebugTrace::ForDuration, 2.0f);

        }
    }
}

void UACMCollisionManagerComponent::ShowDebugTrace(const FVector& StartPos, const FVector& EndPos, const float radius, EDrawDebugTrace::Type DrawDebugType, float duration, FLinearColor DebugColor)
{
	FHitResult hitRes;
	UWorld* world = GetWorld();
	if (world) {
          UKismetSystemLibrary::DrawDebugCylinder(this, StartPos, EndPos, radius, 12, DebugColor, duration);
	}
}

void UACMCollisionManagerComponent::StartTimedSingleTrace_Implementation(const FName& TraceName, float Duration)
{
    UWorld* world = GetWorld();
    if (world) {
        StartSingleTrace(TraceName);
        FTimerHandle timerHandle;
        FTimerDelegate TimerDelegate;

        TFunction<void(void)> lambdaDelegate = [this, TraceName]() { HandleTimedSingleTraceFinished(TraceName); };
        TimerDelegate.BindLambda(lambdaDelegate);
        TraceTimers.Add(TraceName, timerHandle);
        world->GetTimerManager().SetTimer(timerHandle, TimerDelegate, Duration, false);
    }
}

void UACMCollisionManagerComponent::StartAllTimedTraces_Implementation(float Duration)
{
    UWorld* world = GetWorld();
    if (world && !bAllTimedTraceStarted) {
        StartAllTraces();
        world->GetTimerManager().SetTimer(AllTraceTimer, this, 
        &UACMCollisionManagerComponent::HandleAllTimedTraceFinished, Duration, false);
        bAllTimedTraceStarted = true;
    }
}


void UACMCollisionManagerComponent::SetTraceConfig(const FName& traceName, const FTraceInfo& traceInfo) 
{ 
    DamageTraces.Add(traceName, traceInfo); 
}

void UACMCollisionManagerComponent::HandleTimedSingleTraceFinished(const FName& traceEnded)
{  
    if (IsValid(GetOwner())) {
        UWorld* world = GetWorld();
        if (world && TraceTimers.Contains(traceEnded)) {
            StopSingleTrace(traceEnded);
            FTimerHandle* handle = TraceTimers.Find(traceEnded);
            world->GetTimerManager().ClearTimer(*handle);
        }
    }
}

void UACMCollisionManagerComponent::HandleAllTimedTraceFinished()
{
    StopAllTraces();
    if (GetOwner()) {
        UWorld* world = GetWorld();
        if (world && bAllTimedTraceStarted) {
            world->GetTimerManager().ClearTimer(AllTraceTimer);
            bAllTimedTraceStarted = false;
        }
    }
}

bool UACMCollisionManagerComponent::IsTraceActive(const FName& traceName) 
{ 
    return activatedTraces.Contains(traceName); 
}

void UACMCollisionManagerComponent::ApplyDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace)
{
    if (IgnoredActors.Contains(HitResult.GetActor())) {
        return;
    }

    UACMCollisionsFunctionLibrary::PlayImpactEffect(currentTrace.DamageTypeClass, HitResult.PhysMaterial.Get(), HitResult.ImpactPoint, this);
    switch (currentTrace.DamageType) {
        case EDamageType::EPoint:
            ApplyPointDamage(HitResult, currentTrace);
        break;
        case EDamageType::EArea:
            ApplyAreaDamage(HitResult, currentTrace);
        break;
        default:
            ApplyPointDamage(HitResult, currentTrace);
        break;
    }
}

void UACMCollisionManagerComponent::ApplyPointDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace)
{
    if (HitResult.GetActor()->IsValidLowLevel()) {
        const FVector damagerRelativePos = GetOwner()->GetActorLocation() - HitResult.GetActor()->GetActorLocation();
        const float damage = currentTrace.BaseDamage;
        FPointDamageEvent damageInfo;

        damageInfo.DamageTypeClass = currentTrace.DamageTypeClass;
        damageInfo.Damage = currentTrace.BaseDamage;
        damageInfo.HitInfo = HitResult;
        damageInfo.ShotDirection = damagerRelativePos;
        HitResult.GetActor()->TakeDamage(damage, damageInfo, GetActorOwner()->GetInstigatorController(), GetActorOwner());

        OnActorDamaged.Broadcast(HitResult.GetActor());
    }
}

void UACMCollisionManagerComponent::ApplyAreaDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace)
{
    if (HitResult.GetActor()->IsValidLowLevel()) {
        float damage = currentTrace.BaseDamage;
        FRadialDamageEvent damageInfo;

        damageInfo.DamageTypeClass = currentTrace.DamageTypeClass;
        damageInfo.Params.BaseDamage = currentTrace.BaseDamage;
        damageInfo.ComponentHits.Add(HitResult);
        damageInfo.Origin = HitResult.ImpactPoint;

        HitResult.GetActor()->TakeDamage(damage, damageInfo, GetActorOwner()->GetInstigatorController(), GetActorOwner());
        OnActorDamaged.Broadcast(HitResult.GetActor());
    }
}

void UACMCollisionManagerComponent::HandleAreaDamageFinished()
{
    StopCurrentAreaDamage();
}

void UACMCollisionManagerComponent::HandleAreaDamageLooping()
{
    PerformAreaDamage_Single(currentAreaDamage.Location, currentAreaDamage.Radius);
}

void UACMCollisionManagerComponent::PlayTrails_Implementation(const FName& trail)
{
    if (!DamageTraces.Contains(trail) || !damageMesh) {
        return;
    }
    FTraceInfo traceInfo = *DamageTraces.Find(trail);

    if (traceInfo.AttackParticle && ParticleSystemComponents.Contains(trail) && damageMesh->DoesSocketExist(traceInfo.StartSocket)
        && damageMesh->DoesSocketExist(traceInfo.EndSocket)) {

        UParticleSystemComponent* partComp = *ParticleSystemComponents.Find(trail);
        partComp->SetTemplate(traceInfo.AttackParticle);
        partComp->BeginTrails(traceInfo.StartSocket, traceInfo.EndSocket, ETrailWidthMode::ETrailWidthMode_FromCentre, traceInfo.TrailLength);
    }

    if (traceInfo.AttackSound) {
        UGameplayStatics::SpawnSoundAttached(traceInfo.AttackSound, damageMesh, traceInfo.StartSocket);
    }

    if (traceInfo.NiagaraTrail) {
        UNiagaraComponent* niagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(traceInfo.NiagaraTrail, 
            damageMesh, traceInfo.StartSocket, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget,true);
        NiagaraSystemComponents.Add(trail, niagaraComp);
    }
}


void UACMCollisionManagerComponent::StopTrails_Implementation(const FName& trail)
{
    if (ParticleSystemComponents.Contains(trail)) {
        UParticleSystemComponent* partComp = *ParticleSystemComponents.Find(trail);
        partComp->EndTrails();
    }
    if (NiagaraSystemComponents.Contains(trail)) {
        UNiagaraComponent* partComp = *NiagaraSystemComponents.Find(trail);
        if (partComp) {
            partComp->DestroyComponent();
        }
        NiagaraSystemComponents.Remove(trail);
    }
}
