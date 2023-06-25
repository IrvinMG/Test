// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ALSLoadTask.h"
#include "ALSFunctionLibrary.h"
#include "ALSLoadAndSaveSubsystem.h"
#include "ALSSavableInterface.h"
#include "ALSSaveGame.h"
#include "ALSSaveGameSettings.h"
#include "ALSSaveTypes.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Async/TaskGraphInterfaces.h"

void FLoadWorldTask::DoWork()
{

    loadedGame = UGameplayStatics::GetGameInstance(this->world)->GetSubsystem<UALSLoadAndSaveSubsystem>()->GetCurrentSaveGame();

    if (!loadedGame || !world) {
        FinishLoad(false);
        return;
    }

    ToBeDestroyed.Empty();
    FALSLevelData toBeDeserialized;

    if (loadedGame->TryGetLevelData(levelName, toBeDeserialized)) {
        TArray<FALSActorData> actorsData = toBeDeserialized.GetActorsCopy();

        ToBeSpawned = actorsData;
        for (auto actor : LoadableActors) {
            FALSActorData* actorData = actorsData.FindByKey(actor);
            if (actorData) {
                DeserializeActor(actor, *actorData);
                ToBeSpawned.Remove(*actorData);
            } else if (!UALSFunctionLibrary::IsSpecialActor(world, actor)) {
                ToBeDestroyed.Add(actor);
            }
        }

        if (bLoadAll) {
            ReloadPlayer();
        }

        loadedGame->OnLoaded();
        FinishLoad(true);
        return;
    }

    FinishLoad(false);
}

bool FLoadWorldTask::DeserializeActor(AActor* Actor, const FALSActorData& Record)
{
    if (!IsValid(Actor) || !Record.IsValid() || Actor->GetClass() != Record.Class) {
        return false;
    }

    UALSFunctionLibrary::DeserializeActor(Actor, Record);

    SuccessfullyLoadedActors.Add(Actor, FALSActorLoaded(Record.Transform));

    return true;
}

void FLoadWorldTask::FinishLoad(const bool bSuccess)
{
    if (IsInGameThread()) {
        UGameplayStatics::GetGameInstance(this->world)->GetSubsystem<UALSLoadAndSaveSubsystem>()->FinishLoadWork(bSuccess);
    } else {
        FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
            FSimpleDelegateGraphTask::FDelegate::CreateStatic(
                &GFinishLoad, world, ToBeSpawned, SuccessfullyLoadedActors, ToBeDestroyed,
                bSuccess),
            GetStatId(),
            nullptr, ENamedThreads::GameThread);
    }
}

void FLoadWorldTask::ReloadPlayer()
{
    FALSPlayerData outData;
    loadedGame->GetLocalPlayer(outData);
    APlayerController* playerCont = UGameplayStatics::GetPlayerController(world, 0);

    DeserializeActor(playerCont, outData.PlayerController);

    APawn* pawn = UGameplayStatics::GetPlayerPawn(world, 0);
    FALSActorData playerData;
    DeserializeActor(pawn, outData.Pawn);
}
