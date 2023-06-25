// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 


#include "ACFWheeledVehiclePawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ACFEffectsManagerComponent.h"
#include "Perception/AISense_Sight.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "ARSStatisticsComponent.h"
#include "Net/UnrealNetwork.h"
#include "GenericTeamAgentInterface.h"
#include "ACFMountableComponent.h"
#include "ACFMountPointComponent.h"

AACFWheeledVehiclePawn::AACFWheeledVehiclePawn()
{
	bReplicates = true;

	StatisticsComp = CreateDefaultSubobject<UARSStatisticsComponent>(
		TEXT("Statistic Component"));

	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(
			TEXT("Perception Stimuli Component"));
	DamageHandlerComp = CreateDefaultSubobject<UACFDamageHandlerComponent>(
		TEXT("Damage Handler Component"));

	MountComponent = CreateDefaultSubobject<UACFMountableComponent>(TEXT("Mount Component"));

	DismountPoint = CreateDefaultSubobject<UACFMountPointComponent>(TEXT("Enter/Exit Point"));
	DismountPoint->SetupAttachment(RootComponent);
	AIPerceptionStimuliSource->bAutoRegister = true;
	AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	CombatTeam = ETeam::ENeutral;
}

void AACFWheeledVehiclePawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AACFWheeledVehiclePawn, bIsDead);

}

// Called when the game starts or when spawned
void AACFWheeledVehiclePawn::BeginPlay() {

	Super::BeginPlay();
	SetGenericTeamId(uint8(CombatTeam));

	if (UKismetSystemLibrary::IsServer(this)) {

		if (StatisticsComp) {
			StatisticsComp->InitializeAttributeSet();
		}

		if (DamageHandlerComp) {
			DamageHandlerComp->OnOwnerDeath.AddDynamic(
				this, &AACFWheeledVehiclePawn::HandleDeath);
			DamageHandlerComp->InitializeDamageCollisions(CombatTeam);
		}

		EffetsComp = FindComponentByClass<UACFEffectsManagerComponent>();
	}
}
float AACFWheeledVehiclePawn::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return DamageHandlerComp->TakeDamage(this, Damage, DamageEvent, EventInstigator, DamageCauser);
}

FText AACFWheeledVehiclePawn::GetInteractableName_Implementation()
{
	return FText::FromName(VehicleName);
}

FACFDamageEvent AACFWheeledVehiclePawn::GetLastDamageInfo() const
{
	return DamageHandlerComp->GetLastDamageInfo();
}

void AACFWheeledVehiclePawn::HandleDeath()
{
	bIsDead = true;
	OnVehicleDestroyed();
}


void AACFWheeledVehiclePawn::OnVehicleDestroyed_Implementation()
{
	//IMPLEMENT ME IN CHILD CLASSES!
}

void AACFWheeledVehiclePawn::SetGenericTeamId(const FGenericTeamId& InTeamID) {
	/*Super::SetGenericTeamId(InTeamID);*/
	CombatTeam = (ETeam)InTeamID.GetId();
}

FGenericTeamId AACFWheeledVehiclePawn::GetGenericTeamId() const {
	return uint8(CombatTeam);
}


float AACFWheeledVehiclePawn::GetEntityExtentRadius_Implementation() const
{
	FVector origin, extent;
	GetActorBounds(true, origin, extent, false);
	return extent.X;
}

bool AACFWheeledVehiclePawn::CanBeInteracted_Implementation(class APawn* Pawn)
{
	return !MountComponent->IsMounted();
}

void AACFWheeledVehiclePawn::AssignTeamToEntity_Implementation(ETeam team)
{
    CombatTeam = team;

    SetGenericTeamId(uint8(CombatTeam));

    if (DamageHandlerComp) {
        DamageHandlerComp->InitializeDamageCollisions(CombatTeam);
    }

    OnTeamChanged.Broadcast(CombatTeam);
}


