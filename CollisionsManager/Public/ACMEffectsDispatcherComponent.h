// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMTypes.h"
#include "ACMEffectsDispatcherComponent.generated.h"


UCLASS(ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class COLLISIONSMANAGER_API UACMEffectsDispatcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACMEffectsDispatcherComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = ACM)
	class UACMImpactsFXDataAsset* ImpactFXs;
private:

	UFUNCTION(NetMulticast, Reliable, Category = ACM)
	void ClientsPlayEffect(const FActionEffect& effect, class ACharacter* instigator );

	UFUNCTION(NetMulticast, Reliable, Category = ACM)
	void ClientsPlayReplicatedEffect(const FImpactFX& FXtoPlay);

	void SpawnSoundAndParticleAttached(const FActionEffect& effect, class ACharacter* instigator);
    void SpawnSoundAndParticleAtLocation(const FImpactFX& effect);

public:	

	UFUNCTION(BlueprintCallable, Server,  Reliable,  Category = ACM)
	void PlayReplicatedActionEffect(const FActionEffect& effect, class ACharacter* instigator);

	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACM)
    void PlayActionEffectLocally(const FActionEffect& effect, class ACharacter* instigator);

	UFUNCTION(BlueprintCallable, Category = ACM)
	void PlayReplicatedImpact(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, const FVector& impactLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = ACM)
	void PlayReplicatedEffect(const FImpactFX& FXtoPlay);

	UFUNCTION(BlueprintCallable, Category = ACM)
    void PlayEffectLocally(const FImpactFX& effect);


	UFUNCTION(BlueprintCallable, Category = ACM)
	bool TryGetImpactFX(const TSubclassOf<class UDamageType>& damageImpacting, class UPhysicalMaterial* materialImpacted, FBaseFX& outFXtoPlay);

	void Internal_PlayEffect(class ACharacter* instigator, const FActionEffect& effect);

};
