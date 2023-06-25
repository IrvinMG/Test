// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFWeapon.h"
#include <GameFramework/DamageType.h>
#include <Components/SceneComponent.h>
#include "ACMCollisionManagerComponent.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Interfaces/ACFInteractableInterface.h"
#include "Components/ACFTeamManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"


AACFWeapon::AACFWeapon()
{
	HandlePos = CreateDefaultSubobject<USceneComponent>(TEXT("Handle"));
	SetRootComponent(HandlePos);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	Mesh->SetupAttachment(HandlePos);
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp = CreateDefaultSubobject<UACMCollisionManagerComponent>(TEXT("Collisions Manager"));
	LeftHandleIKPos = CreateDefaultSubobject<USceneComponent>(TEXT("Left Hand Handle"));
	LeftHandleIKPos->SetupAttachment(Mesh);
}


void AACFWeapon::BeginPlay()
{
	Super::BeginPlay();
	Mesh->AttachToComponent(HandlePos, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	AlignWeapon();
	if (CollisionComp) {
		CollisionComp->SetupCollisionManager(Mesh);
	}
	
}

void AACFWeapon::OnItemEquipped_Implementation()
{
	Super::OnItemEquipped_Implementation();
	AlignWeapon();
}

void AACFWeapon::OnItemUnEquipped_Implementation()
{
	Super::OnItemUnEquipped_Implementation();
	AlignWeapon();
}

void AACFWeapon::OnWeaponUnsheathed_Implementation()
{

	if (CollisionComp) {
		CollisionComp->SetActorOwner(ItemOwner);
		CollisionComp->SetupCollisionManager(Mesh);
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
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("NO  COLLISION MANAGER ON WEAPON - AACFWeapon"));
	}
}

void AACFWeapon::OnWeaponSheathed_Implementation()
{
	if (CollisionComp)
	{
		CollisionComp->ClearCollisionChannels();
	}
}

void AACFWeapon::Internal_OnWeaponUnsheathed()
{
	AlignWeapon();
	AddModifierToOwner(UnsheathedAttributeModifier);
	OnWeaponUnsheathed();
}

void AACFWeapon::Internal_OnWeaponSheathed()
{
	AlignWeapon();
	RemoveModifierToOwner(UnsheathedAttributeModifier);
	OnWeaponSheathed();
}

void AACFWeapon::AlignWeapon()
{	
	if (HandlePos)
	{
		FHitResult outResult;
		Mesh->SetRelativeTransform(AttachmentOffset, true, &outResult, ETeleportType::TeleportPhysics);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO HANDLE COMPONENT! - ACFWeapon"));
	}
}

