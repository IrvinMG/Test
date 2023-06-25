// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFMeleeWeapon.h"
#include <Particles/ParticleSystemComponent.h>
#include "ACMCollisionManagerComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <Sound/SoundCue.h>


void AACFMeleeWeapon::HandleAttackHit(const FHitResult& HitResult)
{
	OnWeaponHit.Broadcast(HitResult);
}


void AACFMeleeWeapon::OnWeaponUnsheathed_Implementation()
{
	Super::OnWeaponUnsheathed_Implementation();
	if (CollisionComp) {
		if (!CollisionComp->OnCollisionDetected.IsAlreadyBound(this, &AACFMeleeWeapon::HandleAttackHit))
		{
			CollisionComp->OnCollisionDetected.AddDynamic(this, &AACFMeleeWeapon::HandleAttackHit);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("NO  COLLISION MANAGER ON WEAPON"));
	}
}

void AACFMeleeWeapon::OnWeaponSheathed_Implementation()
{
	Super::OnWeaponSheathed_Implementation();
	if (CollisionComp)
	{
		CollisionComp->OnCollisionDetected.RemoveDynamic(this, &AACFMeleeWeapon::HandleAttackHit);
	}
}


AACFMeleeWeapon::AACFMeleeWeapon()
{
	ItemInfo.ItemType = EItemType::MeleeWeapon;
	ItemInfo.Name= FText::FromString("Base Melee Weapon");
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void AACFMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

}


void AACFMeleeWeapon::StartWeaponSwing(const TArray<FName>& traceChannels)
{
	if (CollisionComp) {
		if (traceChannels.Num() == 0) {
			CollisionComp->StartAllTraces();
		}
		else{
			for (const auto& channel : traceChannels) {
				CollisionComp->StartSingleTrace(channel);
			}
		}
	}
}


void AACFMeleeWeapon::StopWeaponSwing()
{
	if (CollisionComp) {
		CollisionComp->StopAllTraces();
	}
}



// void AACFMeleeWeapon::StartEnchantWeaponFX(FGameplayTag Enchantment)
// {
// 	bIsEnchanted = true;
// 	CurrentEnchantment = Enchantment;
// 	PlayFXByTag(Enchantment);
// 	if (GetCurrentWeaponEffect().DamageType)
// 	{
// 		CollisionComp->SetDamageTypeClass(GetCurrentWeaponEffect().DamageType);
// 	}
// }
// 
// void AACFMeleeWeapon::StopEnchantmentFX()
// {
// 	bIsEnchanted = false;
// 	CurrentEnchantment = FGameplayTag();
// 	if (GetCurrentWeaponEffect().DamageType)
// 	{
// 		CollisionComp->SetDamageTypeClass(GetCurrentWeaponEffect().DamageType);
// 	}
// 	StopFX();
// }


/*
void AACFMeleeWeapon::PlayFXByTag_Implementation(const FGameplayTag& fxtag)
{
	FWeaponEffects* vfx = WeaponEffectsByEnchantment.Find(fxtag);

	if (vfx)	{
		UParticleSystem* _effect = vfx->AttackParticle;
		bool bGate = true;
		auto traceMap = CollisionComp->GetDamageTraces();
		int32 index = 0;
		for (const auto& trace : traceMap) {
			if (_effect && Mesh->DoesSocketExist(trace.Value.StartSocket) && Mesh->DoesSocketExist(trace.Value.EndSocket))
			{
				UParticleSystemComponent* ParticleSystemComp = NewObject< UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass());
				ParticleSystemComp->SetupAttachment(Mesh);
				ParticleSystemComp->SetRelativeLocation(FVector::ZeroVector);
				EnchantmentComponents.Add(ParticleSystemComp);
				ParticleSystemComp->RegisterComponent();

				//Calculate Cylinder Height
				FVector BottomLocation = Mesh->GetSocketLocation(trace.Value.StartSocket);
				FVector TopLocation = Mesh->GetSocketLocation(trace.Value.EndSocket);
				VFXCylinderHeight = (BottomLocation.X - TopLocation.X) +
					(BottomLocation.Y - TopLocation.Y) + (BottomLocation.Z - TopLocation.Z);
				FRotator relativeRotator = CollisionComp->GetLineRotation(TopLocation, BottomLocation);
				ParticleSystemComp = UGameplayStatics::SpawnEmitterAttached(_effect, Mesh, TEXT("Enchant Effect"), FVector::ZeroVector, relativeRotator);
				ParticleSystemComp->SetFloatParameter(TEXT("CylinderHeight"), VFXCylinderHeight);
				index++;
				if (bGate && vfx->AttackSound)
				{
					UGameplayStatics::SpawnSoundAttached(vfx->AttackSound, GetMeshComponent(), trace.Value.StartSocket);
					bGate = false;
				}
			}
		}
	}
}

bool AACFMeleeWeapon::PlayFXByTag_Validate(const FGameplayTag& fxtag)
{
	return true;
}

void AACFMeleeWeapon::StopFX_Implementation()
{
	for ( auto EnchantmentComp : EnchantmentComponents)
	{
		EnchantmentComp->DestroyComponent();
	}
}

bool AACFMeleeWeapon::StopFX_Validate()
{
	return true;
}
*/
