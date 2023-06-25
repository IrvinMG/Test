// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Commands/ACFGoThereCommand.h"
#include "ACFAIController.h"
#include <Kismet/KismetSystemLibrary.h>
#include <AITypes.h>
#include <CollisionQueryParams.h>

void UACFGoThereCommand::ExecuteCommand_Implementation(class AACFAIController* controller)
{
	FVector EyesLocation;
	FRotator EyesRotation;
	
	AActor* CharacterOwner = controller->GetLeadActorBK();

	if (CharacterOwner)
	{
		CharacterOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		FVector ShotDirection = EyesRotation.Vector();
		FVector TraceEnd = EyesLocation + (ShotDirection * 50000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CharacterOwner);

		FCollisionObjectQueryParams objQueryParams;
		objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

		FHitResult Hit;
		bool const IsHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyesLocation, TraceEnd, objQueryParams, QueryParams);
		if (IsHit)
		{

			FVector ImpactLocation = Hit.ImpactPoint;

			if (bDrawDebug)
			{
				UKismetSystemLibrary::DrawDebugSphere(controller, ImpactLocation, 100.f, 12.f, FLinearColor::Green, 5.f);
			}
		
			controller->MoveToLocation(ImpactLocation, 50.f, true,true, true, false);

		}
	}

}

void UACFGoThereCommand::EndCommand_Implementation(class AACFAIController* controller)
{

}
