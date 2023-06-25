// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFFunctionLibrary.h"
#include "ACFDeveloperSettings.h"
#include "ACMCollisionManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include "Components//ACFTeamManagerComponent.h"
#include "ACMEffectsDispatcherComponent.h"
#include "Game/ACFDamageCalculation.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFGameMode.h"
#include "Game/ACFGameState.h"
#include "Game/ACFPlayerController.h"
#include "Items/ACFWorldItem.h"
#include "UI/ACFHUD.h"
#include <Components/AudioComponent.h>
#include <Curves/CurveFloat.h>
#include <Engine/DataTable.h>
#include <Engine/LevelStreaming.h>
#include <GameFramework/Actor.h>
#include <GameplayTagContainer.h>
#include <GameplayTagsManager.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <NavigationSystem.h>
#include <Sound/SoundCue.h>
#include "Sound/SoundClass.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Game/ACFTypes.h"
#include "Camera/CameraShakeBase.h"
#include "Components/ACFEffectsManagerComponent.h"
#include "ACFTeamsConfigDataAsset.h"


EACFDirection UACFFunctionLibrary::GetActorsRelativeDirection(const AActor* hitDealer, const AActor* receiver)
{
    if (!hitDealer || !receiver) {
        return EACFDirection::Back;
    }

    const FVector damageDealerRelative = hitDealer->GetActorLocation()
        - receiver->GetActorLocation();

    const FVector hitDir = damageDealerRelative.GetUnsafeNormal(); 
    return GetDirectionFromInput(receiver, hitDir);

}

EACFDirection UACFFunctionLibrary::GetDirectionFromInput(const AActor* actor, const FVector& direction)
{

    if (!actor) {
		UE_LOG(LogTemp, Error, TEXT("INVALID ACTOR! - UACFFunctionLibrary::GetDirectionFromInput "));

        return EACFDirection::Front;
    }
	const float forwardDot = FVector::DotProduct(actor->GetActorForwardVector(), direction);
	const float rightDot = FVector::DotProduct(actor->GetActorRightVector(), direction);
    if (forwardDot >= 0.707f) {
		return EACFDirection::Front;
    }
    else if (rightDot >= 0.707f) {
		return EACFDirection::Right;
    }
    else if (rightDot <= -0.707f) {
		return EACFDirection::Left;

    }
    else {
		return EACFDirection::Back;
    }

    return EACFDirection::Front;
}

EACFDirection UACFFunctionLibrary::GetHitDirectionByHitResult(const AActor* hitDealer, const FHitResult& hitResult)
{
    ensure(hitDealer);

    AActor* receiver = hitResult.GetActor();

    return GetActorsRelativeDirection(hitDealer, receiver);
}

FVector UACFFunctionLibrary::GetActorsRelativeDirectionVector(const FACFDamageEvent& damageEvent)
{
    if (damageEvent.DamageDealer && damageEvent.DamageReceiver) {
        const FVector damageDealerRelative = damageEvent.DamageReceiver->GetActorLocation() - damageEvent.DamageDealer->GetActorLocation();

        return damageDealerRelative.GetUnsafeNormal();
    }

    return FVector();
}



FVector UACFFunctionLibrary::GetActorsOppositeRelativeDirection(const FACFDamageEvent& damageEvent)
{
    return -GetActorsRelativeDirectionVector(damageEvent);
}


FGameplayTag UACFFunctionLibrary::GetDefaultDeathState()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DefaultDeathState;
    }

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetDefaultHitState()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DefaultHitState;
    }

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetHealthTag()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->HealthTag;
    }

    return FGameplayTag();
}


float UACFFunctionLibrary::ReduceDamageByPercentage(float Damage, float Percentage)
{
    const float _deviation = Damage * Percentage / 100;
    return FMath::Clamp(Damage - _deviation, 0.f, BIG_NUMBER);
}

void UACFFunctionLibrary::PlayImpactEffect(const FImpactEffect& effect, const FVector& impactLocation, class AActor* instigator, const UObject* WorldContextObject)
{

    if (effect.CameraShake && effect.bShouldShakeCamera && WorldContextObject) {
        UGameplayStatics::PlayWorldCameraShake(WorldContextObject, effect.CameraShake, impactLocation, 0.f, effect.ShakeRadius, effect.ShakeFalloff);
    }

    if (effect.bShouldMakeDamage && instigator) {
        UACMCollisionManagerComponent* collComp = instigator->FindComponentByClass<UACMCollisionManagerComponent>();

        if (!collComp) {
            return;
        }

        collComp->StartTimedSingleTrace(effect.DamageTraceName, effect.DamageTraceDuration);
    }
}

void UACFFunctionLibrary::PlayActionEffect(const FActionEffect& effect, ACharacter* instigator, const UObject* WorldContextObject)
{
    const AACFGameState* gameState = UACFFunctionLibrary::GetACFGameState(WorldContextObject);

    if (gameState) {
        UACMEffectsDispatcherComponent* EffectComp = gameState->GetEffectsComponent();
        if (EffectComp) {
			EffectComp->PlayReplicatedActionEffect(effect, instigator);
			return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Missing ACFGameState! - UACFFunctionLibrary "));
}

void UACFFunctionLibrary::PlayActionEffectLocally(const FActionEffect& effect, class ACharacter* instigator, const UObject* WorldContextObject)
{
     AGameStateBase* gameState = UGameplayStatics::GetGameState(WorldContextObject);

    if (gameState) {
         UACMEffectsDispatcherComponent* EffectComp = gameState->FindComponentByClass <UACMEffectsDispatcherComponent>();
         if (EffectComp) {
             EffectComp->PlayActionEffectLocally(effect, instigator);
             return;
        }
         ensure(EffectComp);
    }

    UE_LOG(LogTemp, Warning, TEXT("Missing ACFGameState! - UACFFunctionLibrary "));
}

void UACFFunctionLibrary::PlayFootstepEffect(class APawn* instigator, FName footBone /*= NAME_None*/, const UObject* WorldContextObject)
{
    UACFEffectsManagerComponent* effectsComp = instigator->FindComponentByClass<UACFEffectsManagerComponent>();
    if (effectsComp) {
        effectsComp->TriggerFootstepFX(footBone);
    }
}

EBattleType UACFFunctionLibrary::GetBattleType(const UObject* WorldContextObject)
{
    const AACFGameState* gameState = Cast<AACFGameState>(UGameplayStatics::GetGameState(WorldContextObject));

    if (gameState) {
        return gameState->GetTeamManager()->GetBattleType();
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Trying to get battle type from client! COnfigurations must be done on server!! - UACFFunctionLibrary "));
    }
    return EBattleType::ETeamBased;
}



bool UACFFunctionLibrary::AreEnemyTeams(const UObject* WorldContextObject, ETeam SelfTeam, ETeam TargetTeam)
{
    UACFTeamManagerComponent* teamManager = GetACFTeamManager(WorldContextObject);
    if (teamManager) {
        return teamManager->IsEnemyTeam(SelfTeam, TargetTeam);
    }
    UE_LOG(LogTemp, Error, TEXT("Missing ACFGameState! - UACFFunctionLibrary::AreEnemyTeams "));
    return false;
}

bool UACFFunctionLibrary::TryGetTeamsConfig(const UObject* WorldContextObject, TMap<ETeam, FTeamInfo>& outConfig)
{
    UACFTeamManagerComponent* teamManager = GetACFTeamManager(WorldContextObject);
    if (teamManager) {
        UACFTeamsConfigDataAsset* teamConfig = teamManager->GetTeamsConfiguration();
        if (teamConfig) {
            outConfig = teamConfig->GetTeamsConfig();
            return true;
        }
    }   
    UE_LOG(LogTemp, Error, TEXT("Missing Teams Configuration in Game State! - UACFFunctionLibrary::TryGetTeamsConfig "));

    return false;
}



float UACFFunctionLibrary::CalculateDistanceBetweenActors(const AActor* characterA, const AActor* characterB)
{
	const ACharacter* owner = Cast<ACharacter>(characterA);
	const ACharacter* target = Cast<ACharacter>(characterB);

    if (owner && target) {
        return CalculateDistanceBetweenCharactersExtents(owner, target);
    }
    return owner->GetDistanceTo(target);
}


FVector UACFFunctionLibrary::GetPointAtDirectionAndDistanceFromActor(const AActor* targetActor, const FVector& direction, float distance, bool bShowDebug /*= false*/)
{
    if (!targetActor) {
        return FVector();
    }
    const FVector actorLoc = targetActor->GetActorLocation();
    FVector normalizedDir = direction;
   
    normalizedDir.Normalize();
    const FVector additivePos = (normalizedDir * distance);
    FVector targetPos = actorLoc + additivePos;
    targetPos.Z = targetActor->GetActorLocation().Z;

    if (bShowDebug) {
		UKismetSystemLibrary::DrawDebugSphere(targetActor, targetPos, 100.f, 12, FLinearColor::Yellow,1.f, 1.f);
    }

    return targetPos;
}

float UACFFunctionLibrary::CalculateDistanceBetweenCharactersExtents(const ACharacter* characterA, const ACharacter* characterB)
{
    if (characterA && characterB) {
        const FBox aBox = characterA->GetMesh()->Bounds.GetBox();
        const FBox bBox = characterB->GetMesh()->Bounds.GetBox();

        const float dist = characterA->GetDistanceTo(characterB);
        return dist - aBox.GetExtent().X - bBox.GetExtent().X;
    }
    return -1.f;
}

float UACFFunctionLibrary::GetCharacterExtent(const ACharacter* characterA)
{
    if (characterA) {
        const FBox aBox = characterA->GetMesh()->Bounds.GetBox();
        return  aBox.GetExtent().X ;
    }
    return -1.f;
}

float UACFFunctionLibrary::GetCharacterDistantToPointConsideringExtent(const ACharacter* characterA, const FVector& point)
{
    if (characterA ) {
        const FBox aBox = characterA->GetMesh()->Bounds.GetBox();

        const float dist = FVector::Dist(characterA->GetActorLocation(), point);
        return dist - aBox.GetExtent().X ;
    }
    return -1.f;
}

float UACFFunctionLibrary::CalculateAngleBetweenVectors(const FVector& vectorA, const FVector& vectorB)
{
    return FMath::RadiansToDegrees(acosf(FVector::DotProduct(vectorA, vectorB)));
}

class AACFHUD* UACFFunctionLibrary::GetACFHUD(const UObject* WorldContextObject)
{
    APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);

    return Cast<AACFHUD>(playerController->GetHUD());
}

class AACFPlayerController* UACFFunctionLibrary::GetLocalACFPlayerController(const UObject* WorldContextObject)
{
    return Cast<AACFPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}

class AACFGameMode* UACFFunctionLibrary::GetACFGameMode(const UObject* WorldContextObject)
{
    return Cast<AACFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}

class AACFGameState* UACFFunctionLibrary::GetACFGameState(const UObject* WorldContextObject)
{
    return Cast<AACFGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

class AACFCharacter* UACFFunctionLibrary::GetLocalACFPlayerCharacter(const UObject* WorldContextObject)
{
    return Cast<AACFCharacter>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
}

UACFTeamManagerComponent* UACFFunctionLibrary::GetACFTeamManager(const UObject* WorldContextObject)
{
    AACFGameState* GameState = GetACFGameState(WorldContextObject);
    if (GameState) {
        return GameState->GetTeamManager();
    }
    UE_LOG(LogTemp, Error, TEXT("Missing Game State - UACFFunctionLibrary::GetACFTeamManager"));
    return nullptr;
}

bool UACFFunctionLibrary::ShouldExecuteAction(const FActionChances& action, const AACFCharacter* characterOwner)
{
	return characterOwner && characterOwner->GetActionsComponent() &&
		characterOwner->GetActionsComponent()->CanExecuteAction(action.ActionTag) &&
		FMath::RandRange(0.f, 100.f) < action.ChancePercentage && (
			(uint8)characterOwner->GetRelativeTargetDirection(characterOwner->GetTarget()) == (uint8)action.AllowedFromDirection ||
			action.AllowedFromDirection == EActionDirection::EveryDirection
			);
}
