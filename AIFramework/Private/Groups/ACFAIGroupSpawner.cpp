// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Groups/ACFAIGroupSpawner.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include "ACFAIController.h"
#include <Components/SplineComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Actors/ACFCharacter.h"
#include "Components/ACFGroupAIComponent.h"
#include "Game/ACFTypes.h"
#include "ACFPatrolPath.h"
#include "Groups/ACFGroupAIController.h"
#include "TimerManager.h"
#include "ACFAITypes.h"

// Sets default values
AACFAIGroupSpawner::AACFAIGroupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// initalize spline component
	
	
	AIGroupComponent = CreateDefaultSubobject<UACFGroupAIComponent>(TEXT("Group Componnet"));
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Componnet"));
    SetRootComponent(AIGroupComponent);

    SplineComponent->SetupAttachment(AIGroupComponent);
	AIControllerClass = AACFGroupAIController::StaticClass();

}

// Called when the game starts or when spawned
void AACFAIGroupSpawner::BeginPlay()
{
	Super::BeginPlay();

	SetGenericTeamId(uint8(AIGroupComponent->GetCombatTeam()));

	patrolPath = GetWorld()->SpawnActor<AACFPatrolPath>(AACFPatrolPath::StaticClass(), GetTransform());

	TArray<FVector> splinePoints;
	for (int32 index = 0; index < SplineComponent->GetNumberOfSplinePoints(); ++index) {
		const FVector splinePoint = SplineComponent->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);
		splinePoints.Add(splinePoint);
	}

	patrolPath->SetSplinePoints(splinePoints);
	patrolPath->SetPatrolConfig(PatrolConfig);
	AIGroupComponent->OnAgentsSpawned.AddDynamic(this, &AACFAIGroupSpawner::StartUpdatePos);
	AIGroupComponent->OnAllAgentDeath.AddDynamic(this, &AACFAIGroupSpawner::StopUpdatePos);
	AActor* Actor = GetOwner();
	if ( AIGroupComponent && Actor && Actor->HasAuthority() && patrolPath) {
		AIGroupComponent->SetPatrolPath(patrolPath);
		if (bSpawnOnBeginPlay) {
			AIGroupComponent->SpawnGroup();
		}
	}
}

FVector AACFAIGroupSpawner::GetPatrolPoint(int32 patrolIndex) const
{
	return GetSplineComponent()->GetLocationAtSplinePoint(patrolIndex, ESplineCoordinateSpace::World);
}

int32 AACFAIGroupSpawner::GetGroupSize() const
{
    return GetAIGroupComponent()->GetGroupSize();
}

bool AACFAIGroupSpawner::GetAgentWithIndex(int32 index, FAIAgentsInfo& outAgent) const
{
	if (AIGroupComponent) {
		return AIGroupComponent->GetAgentByIndex(index, outAgent);
	}
	return false;
}

AACFCharacter* AACFAIGroupSpawner::GetAgentNearestTo(const FVector& location) const 
{
	if (AIGroupComponent) {
		return AIGroupComponent->GetAgentNearestTo(location);	
	}
	return nullptr;
}


void AACFAIGroupSpawner::StartUpdatePos()
{
    if (bCheckPos || !bUpdateSpawnerPosition) {
		return;
	}

	UWorld* world = GetWorld();
	if (world) {
		world->GetTimerManager().SetTimer(
			timerHandle, this, &AACFAIGroupSpawner::AdjustSpawnerPos,
			UpdateGroupPositionTimeInterval, true);
			bCheckPos = true;
	}	
}

void AACFAIGroupSpawner::StopUpdatePos()
{
	timerHandle.Invalidate();
	bCheckPos = false;
}

void AACFAIGroupSpawner::AdjustSpawnerPos()
{
	if (AIGroupComponent->GetGroupSize() == 0) {
		StopUpdatePos();
		return;
	}
	
	FAIAgentsInfo agentInfo;

	if (AIGroupComponent->GetAgentByIndex(0, agentInfo) && agentInfo.AICharacter) {
		SetActorRotation(agentInfo.AICharacter->GetActorRotation());
	}

	SetActorLocation(AIGroupComponent->GetGroupCentroid());
}

void AACFAIGroupSpawner::SetGenericTeamId(const FGenericTeamId& InTeamID) {
	/*Super::SetGenericTeamId(InTeamID);*/
	CombatTeam = (ETeam)InTeamID.GetId();
}

FGenericTeamId AACFAIGroupSpawner::GetGenericTeamId() const {
	return uint8(CombatTeam);
}
