// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFPatrolPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AACFPatrolPath::AACFPatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Componet"));

}

// Called when the game starts or when spawned
void AACFPatrolPath::BeginPlay()
{
	Super::BeginPlay();
	
}

void AACFPatrolPath::AddSplinePoint(const FVector& worldPos)
{
	SplineComp->AddSplinePoint(worldPos, ESplineCoordinateSpace::World);
}

void AACFPatrolPath::SetSplinePoints(const TArray<FVector>& worldPos)
{
	SplineComp->SetSplinePoints(worldPos, ESplineCoordinateSpace::World);
}
