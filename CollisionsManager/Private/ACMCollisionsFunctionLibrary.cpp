// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 


#include "ACMCollisionsFunctionLibrary.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "ACMEffectsDispatcherComponent.h"

UACMEffectsDispatcherComponent* UACMCollisionsFunctionLibrary::GetEffectDispatcher(const UObject* WorldContextObject)
{
	const AGameStateBase* gameState = UGameplayStatics::GetGameState(WorldContextObject);

	if (gameState) {
		UACMEffectsDispatcherComponent* effectManag = gameState->FindComponentByClass<class UACMEffectsDispatcherComponent>();
		if (effectManag) {
			return effectManag;
		}	
	}
	UE_LOG(LogTemp, Error, TEXT("Missing  Effects Dispatcher Component in GAME STATE! - UACMCollisionsFunctionLibrary::GetEffectDispatcher "));

	return nullptr;
}

void UACMCollisionsFunctionLibrary::PlayImpactEffect(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, const FVector& impactLocation,  const UObject* WorldContextObject)
{
	UACMEffectsDispatcherComponent* effectDips = UACMCollisionsFunctionLibrary::GetEffectDispatcher(WorldContextObject);
	if (effectDips) {
		effectDips->PlayReplicatedImpact(damageImpacting, materialImpacted, impactLocation);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Missing  Effects Dispatcher Component in GAME STATE! - UACMCollisionsFunctionLibrary "));

}

void UACMCollisionsFunctionLibrary::PlayReplicatedEffect(const FImpactFX& FXtoPlay, const UObject* WorldContextObject)
{
	UACMEffectsDispatcherComponent* effectDips = UACMCollisionsFunctionLibrary::GetEffectDispatcher(WorldContextObject);
	if (effectDips) {
		effectDips->PlayReplicatedEffect(FXtoPlay);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Missing  Effects Dispatcher Component in GAME STATE! - UACMCollisionsFunctionLibrary "));
}

void UACMCollisionsFunctionLibrary::PlayEffectLocally(const FImpactFX& FXtoPlay, const UObject* WorldContextObject)
{
    UACMEffectsDispatcherComponent* effectDips = UACMCollisionsFunctionLibrary::GetEffectDispatcher(WorldContextObject);
    if (effectDips) {
        effectDips->PlayEffectLocally(FXtoPlay);
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Missing  Effects Dispatcher Component in GAME STATE! - UACMCollisionsFunctionLibrary "));
}
