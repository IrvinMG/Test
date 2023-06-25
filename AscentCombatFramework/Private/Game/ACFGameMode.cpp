// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFGameMode.h"
#include "ACMCollisionsMasterComponent.h"
#include "Components/ACFRagdollMasterComponent.h"
#include "Game/ACFPlayerController.h"
#include <Kismet/GameplayStatics.h>


AACFGameMode::AACFGameMode()
{
	CollisionManager = CreateDefaultSubobject<UACMCollisionsMasterComponent>(TEXT("Collision Master Comp"));
	RagdollManager = CreateDefaultSubobject<UACFRagdollMasterComponent>(TEXT("Ragdoll Master Comp"));
}

// void AACFGameMode::SpawnPlayersCompanions()
// {
// 	TArray<class AACFPlayerController*> pcs = GetAllPlayerControllers();
// 
// 	for (AACFPlayerController* pc : pcs) {
// 		pc->GetCompanionsComponent()->SpawnGroup();
// 	}
// }

TArray<class AACFPlayerController*> AACFGameMode::GetAllPlayerControllers() 
{
	TArray<class AACFPlayerController*> playerContrs;
	int32 numPlayer = GetNumPlayers();

	for (int32 i = 0; i < numPlayer; i++) {
		 APlayerController* pc = UGameplayStatics::GetPlayerController(this,i );
		if (pc) {
			 AACFPlayerController* acfpc = Cast< AACFPlayerController>(pc);
			if (acfpc) {
				playerContrs.Add(acfpc);
			}
		}
	}

	return playerContrs;
}
