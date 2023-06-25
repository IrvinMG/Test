// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFProjectile.h"
#include "ACMCollisionManagerComponent.h"
#include "Items/ACFRangedWeapon.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Actor.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include "Interfaces/ACFInteractableInterface.h"
#include "Components/ACFTeamManagerComponent.h"
#include "Interfaces/ACFEntityInterface.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"

// Sets default values
AACFProjectile::AACFProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    // Create A Default Root Component as a container
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("RootComp"));
    SetRootComponent(SphereComp);

    CollisionComp = CreateDefaultSubobject<UACMCollisionManagerComponent>(TEXT("Collision Manager Comp"));
    // Attach Mesh component to root component
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComp"));
    MeshComp->SetupAttachment(SphereComp);
    MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    bReplicates = true;

    SetReplicateMovement(true);
    ItemInfo.Name = FText::FromString("Base Projectile");
    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));

    ProjectileMovementComp->UpdatedComponent = SphereComp;
    ProjectileMovementComp->InitialSpeed = 4000.f;
    ProjectileMovementComp->MaxSpeed = 5000.0f;
    ProjectileMovementComp->bRotationFollowsVelocity = true;
    ProjectileMovementComp->bShouldBounce = false;
    ProjectileMovementComp->bAutoActivate = false;

    bIsFlying = false;

    ItemInfo.ItemType = EItemType::Projectile;
    SphereComp->SetSphereRadius(4.0f);

}

// Called when the game starts or when spawned
void AACFProjectile::BeginPlay()
{
    Super::BeginPlay();
    if (CollisionComp) {
        CollisionComp->SetActorOwner(ItemOwner);
        CollisionComp->SetupCollisionManager(MeshComp);
    }
    ProjectileMovementComp->SetActive(true);
}

void AACFProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (CollisionComp) {
        CollisionComp->StopAllTraces();
    }
}

void AACFProjectile::Internal_OnEquipped(class ACharacter* _owner)
{
    Super::Internal_OnEquipped(_owner);
    if (MeshComp) {
        MeshComp->SetHiddenInGame(true);
     }
}

void AACFProjectile::SetupProjectile(class APawn* inOwner)
{
    if (inOwner) {
        bIsFlying = true;
        ItemOwner = inOwner;
        SetLifeSpan(ProjectileLifespan);
    } else {
        bIsFlying = false;
    }
}

void AACFProjectile::ActivateDamage()
{
    if (CollisionComp) {
        CollisionComp->SetActorOwner(ItemOwner);
        CollisionComp->SetupCollisionManager(MeshComp);

		const bool bImplements = ItemOwner->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
		if (bImplements) {
			const ETeam combatTeam = IACFEntityInterface::Execute_GetEntityCombatTeam(ItemOwner);
            const AGameStateBase* gameState = UGameplayStatics::GetGameState(this);
			const UACFTeamManagerComponent* teamManager = gameState->FindComponentByClass<UACFTeamManagerComponent>();
			if (teamManager) {
				CollisionComp->SetCollisionChannels(teamManager->GetEnemiesCollisionChannels(combatTeam));
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("NO  TEAM MANAGER MANAGER ON GAMESTATE! - AACFProjectile"));
			}
		}

        CollisionComp->StartAllTraces();
        CollisionComp->OnCollisionDetected.AddDynamic(this, &AACFProjectile::HandleAttackHit);
    }
}

void AACFProjectile::MakeStatic()
{
    ProjectileMovementComp->SetActive(false);
    ProjectileMovementComp->Velocity = FVector::ZeroVector;
    SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
    if (CollisionComp) {
        CollisionComp->StopAllTraces();
    }
}

void AACFProjectile::HandleAttackHit( const FHitResult& HitResult)
{
     ACharacter* damagedActor = Cast<ACharacter>(HitResult.GetActor());

    if (bAttachOnHit) {
        const FVector AttachLocation = GetActorLocation() + (GetActorForwardVector() * PenetrationLevel);
        FActorSpawnParameters spawnParam;
        spawnParam.Owner = damagedActor;
        spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AACFProjectile* copyProjectile = GetWorld()->SpawnActor<AACFProjectile>(this->GetClass(), AttachLocation, GetActorRotation(), spawnParam);
        copyProjectile->MakeStatic();
        const FAttachmentTransformRules rule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
        if (damagedActor) {
            copyProjectile->AttachToComponent(damagedActor->GetMesh(), rule, HitResult.BoneName);
        } else {
            copyProjectile->AttachToComponent(HitResult.Component.Get(), rule);
        }
        copyProjectile->SetActorRotation(GetActorRotation());
        copyProjectile->SetLifeSpan(AttachedLifespan);
        CollisionComp->StopAllTraces();
        Destroy();
    }
}
