// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "ACMTypes.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include <Engine/EngineTypes.h>
#include <GameFramework/DamageType.h>
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ACMCollisionManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollisionDetected, const FHitResult&, HitResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDamaged, AActor*, damageReceiver);

UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class COLLISIONSMANAGER_API UACMCollisionManagerComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACMCollisionManagerComponent();
       
protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM| Debug")
    EDebugType ShowDebugInfo;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM| Debug")
    FLinearColor DebugInactiveColor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM| Debug")
    FLinearColor DebugActiveColor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM")
    bool bAllowMultipleHitsPerSwing;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	/** The actors to be ignore */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM")
	TArray<class AActor*> IgnoredActors;

	/** If this hit should Ignore component's owner. You can set your Owner with SetActorOwner*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM")
	bool bIgnoreOwner = true;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM|Traces")
    TMap<FName, FTraceInfo> DamageTraces;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM|Traces")
    FBaseTraceInfo SwipeTraceInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACM|Traces")
    FBaseTraceInfo AreaDamageTraceInfo;

public:
    UFUNCTION(BlueprintCallable, Category = ACM)
    void SetupCollisionManager(class UMeshComponent* inDamageMesh);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
	void StartAreaDamage(const FVector& damageCenter, float damageRadius, float damageInterval = 1.f);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
	void StopCurrentAreaDamage();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void PerformAreaDamage_Single(const FVector& damageCenter, float damageRadius);

    /*Not replicated version of perform area damage that only works locally but gives bacj the hit result*/
    UFUNCTION(BlueprintCallable, Category = ACM)
    void PerformAreaDamage_Single_Local(const FVector& damageCenter, float damageRadius, TArray<FHitResult>& outHits);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
	void PerformAreaDamageForDuration(const FVector& damageCenter, float damageRadius, float duration, float damageInterval = 1.f);

    UFUNCTION(BlueprintCallable, Category = ACM)
    void AddActorToIgnore(class AActor* ignoredActor);

    UFUNCTION(BlueprintCallable, Category = ACM)
    void AddCollisionChannel(TEnumAsByte<ECollisionChannel> inTraceChannel);

    UFUNCTION(BlueprintCallable, Category = ACM)
    void SetCollisionChannels(TArray<TEnumAsByte<ECollisionChannel>> inTraceChannels);

    UFUNCTION(BlueprintCallable, Category = ACM)
    void ClearCollisionChannels();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void PerformSwipeTraceShot(const FVector& start, const FVector& end, float radius = 0.f);

    /*Not repliacted version of perform swipe trace that only works locally but gives back the hit result*/
    UFUNCTION(BlueprintCallable, Category = ACM)
    void PerformSwipeTraceShot_Local(const FVector& start, const FVector& end, float radius, FHitResult& outHit );

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StartAllTraces();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StopAllTraces();

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StartSingleTrace(const FName& Name);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StopSingleTrace(const FName& Name);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StartTimedSingleTrace(const FName& TraceName, float Duration);

    UFUNCTION(Server, Reliable, BlueprintCallable, Category = ACM)
    void StartAllTimedTraces(float Duration);

    UFUNCTION(NetMulticast, Reliable, Category = ACM)
    void PlayTrails(const FName& trail);

    UFUNCTION(NetMulticast,  Reliable, Category = ACM)
    void StopTrails(const FName& trail);

    UFUNCTION(BlueprintCallable, Category = ACM)
    TMap<FName, FTraceInfo> GetDamageTraces() const
    {
        return DamageTraces;
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = ACM)
    bool IsTraceActive(const FName& traceName);

    /*used to override the default owner. Useful if this component is used in a weapon but
	you want the Damage dealer is actually your character and NOT the weapon itself*/
    UFUNCTION(BlueprintCallable, Category = ACM)
    void SetActorOwner(AActor* newOwner)
    {
        actorOwner = newOwner;
    };

    UFUNCTION(BlueprintPure, Category = ACM)
    FORCEINLINE AActor* GetActorOwner() const
    {
        return actorOwner != nullptr ? actorOwner : GetOwner();
    };

    UFUNCTION(BlueprintCallable, Category = ACM)
    void SetTraceConfig(const FName& traceName, const FTraceInfo& traceInfo);

    UFUNCTION(BlueprintCallable, Category = ACM)
    bool TryGetTraceConfig(const FName& traceName, FTraceInfo& outTraceInfo) const
    {
        if (DamageTraces.Contains(traceName)) {
            outTraceInfo = *DamageTraces.Find(traceName);
            return true;
        }
        return false;
    };

    UPROPERTY(BlueprintAssignable, Category = ACM)
    FOnCollisionDetected OnCollisionDetected;

    UPROPERTY(BlueprintAssignable, Category = ACM)
    FOnActorDamaged OnActorDamaged;

    FRotator GetLineRotation(FVector start, FVector end);

    void UpdateCollisions();

private:
    UPROPERTY()
    class AActor* actorOwner;

    UPROPERTY()
    class UMeshComponent* damageMesh;

    UPROPERTY()
    TMap<FName, FTraceInfo> activatedTraces;

    UPROPERTY()
    TArray<FName> pendingDelete;

    UPROPERTY()
    TMap<FName, FHitActors> alreadyHitActors;

    bool bIsStarted = false;

    void DisplayDebugTraces();

    void ShowDebugTrace(const FVector& StartPos, const FVector& EndPos, const float radius, EDrawDebugTrace::Type DrawDebugType , float duration, FLinearColor DebugColor = FLinearColor::Red);

    UFUNCTION()
    void HandleTimedSingleTraceFinished(const FName& traceEnded);

    UFUNCTION()
    void HandleAllTimedTraceFinished();

    UPROPERTY()
    TMap<FName, class UParticleSystemComponent*> ParticleSystemComponents;

    UPROPERTY()
    TMap<FName, class UNiagaraComponent*> NiagaraSystemComponents;

    void ApplyDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace);

    void ApplyPointDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace);

    void ApplyAreaDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace);

    UPROPERTY()
    FTimerHandle AllTraceTimer;

	UPROPERTY()
	FTimerHandle AreaDamageTimer;

	UPROPERTY()
	FTimerHandle AreaDamageLoopTimer;

	UFUNCTION()
	void HandleAreaDamageFinished();

	UFUNCTION()
	void HandleAreaDamageLooping();

	UPROPERTY()
    FAreaDamageInfo currentAreaDamage;

    UPROPERTY()
    TMap<FName, FTimerHandle> TraceTimers;

    UPROPERTY()
    bool bSingleTimedTraceStarted = false;

    UPROPERTY()
    bool bAllTimedTraceStarted = false;

    void SetStarted(bool inStarted);
};
