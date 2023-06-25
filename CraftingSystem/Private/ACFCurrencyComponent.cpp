// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFCurrencyComponent.h"
#include "ARSStatisticsComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Game/ACFFunctionLibrary.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Game/ACFPlayerController.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFCurrencyComponent::UACFCurrencyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UACFCurrencyComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UACFCurrencyComponent, CurrencyAmount);
}

void UACFCurrencyComponent::RemoveCurrency_Implementation(float amount)
{
	CurrencyAmount -= amount;
}

bool UACFCurrencyComponent::RemoveCurrency_Validate(float amount)
{
	return true;
}

void UACFCurrencyComponent::AddCurrency_Implementation(float amount)
{
	CurrencyAmount += amount;
}

bool UACFCurrencyComponent::AddCurrency_Validate(float amount)
{
	return true;
}

// Called when the game starts
void UACFCurrencyComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bDropCurrencyToPlayerKillerOnOwnerDeath && UKismetSystemLibrary::IsServer(this)) {
		UARSStatisticsComponent* statComp = GetOwner()->FindComponentByClass<UARSStatisticsComponent>();

		if (statComp)	{
			statComp->OnStatisiticReachesZero.AddDynamic(this, &UACFCurrencyComponent::HandleStatReachedZero);
		}
	}
}

void UACFCurrencyComponent::HandleStatReachedZero(FGameplayTag stat)
{
	if (UKismetSystemLibrary::IsServer(this) && UACFFunctionLibrary::GetHealthTag() == stat) {
		UACFDamageHandlerComponent* damageComp = GetOwner()->FindComponentByClass<UACFDamageHandlerComponent>();
			if (damageComp->GetLastDamageInfo().DamageDealer) {
				const AACFPlayerController* playerContr = Cast< AACFPlayerController>(damageComp->GetLastDamageInfo().DamageDealer->GetController());
				if (playerContr) {
					UACFCurrencyComponent* currency =playerContr->FindComponentByClass< UACFCurrencyComponent>();
					if (currency) {
						currency->AddCurrency(CurrencyAmount);
						RemoveCurrency(CurrencyAmount);
					}

				}
			}
	}
}


