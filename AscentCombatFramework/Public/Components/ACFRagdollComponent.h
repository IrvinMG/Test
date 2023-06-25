// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/ACFTypes.h"
#include "Actors/ACFCharacter.h"
#include "Game/ACFDamageType.h"
#include "ACFRagdollComponent.generated.h"

struct FACFDamageEvent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRagdollStateChanged, bool, bIsInRagdoll);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFRagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFRagdollComponent();


	void UpdateOwnerLocation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bTestRagdollOnHit= true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bTestRagdollOnHit), Category = ACF)
	float ImpulseResistance = 850.f;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float DefaultRagdollDuration = 5.f;

	/*If character owner is not kinematic, after the ragdoll ends it needs to be reattached
	to the actual capsule. Just check if your physics asset has it roots to set as kinematic*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bIsKinematic = true;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	FName PelvisBone = "Pelvis";

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	class UAnimMontage* GetUpFront;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	class UAnimMontage* GetUpBack;

public:	

	UFUNCTION(BlueprintCallable, Category = ACF)
	void GoRagdollFromDamage(const FACFDamageEvent& damageEvent, float RagdollDuration = -1.f);
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	void GoRagdoll(const FRagdollImpulse& impulse);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void GoRagdollForDuration(const FRagdollImpulse& impulse, float ragdollDuration = -1.f);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void RecoverFromRagdoll();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool IsInRagDoll() const { return bIsRagdoll; }

	UPROPERTY(BlueprintAssignable)
	FOnRagdollStateChanged OnRagdollStateChanged;

private:

	UFUNCTION()
	void TerminateRagdoll();

	FTransform beforeRagdollTransform;

	bool bIsRagdoll = false;
	   
	class USkeletalMeshComponent* GetMesh() const {
		return characterOwner ? characterOwner->GetMesh() : nullptr;
	}
	
	class UCapsuleComponent* GetCapsuleComponent() const {
		return characterOwner ? characterOwner->GetCapsuleComponent() : nullptr;
	}

	UPROPERTY()
	AACFCharacter* characterOwner;

	UFUNCTION()
	void HandleDamageReceived(const FACFDamageEvent& damageEvent);


	bool IsFaceUp();

	void SetIsRagdoll(bool inIsRagdoll);

	class UAnimMontage* GetGetUpMontage();

	FCollisionResponseContainer oldConfig;
        // 	bool bGettingUp = false;
// 	float blendPower = 1.f;
};
