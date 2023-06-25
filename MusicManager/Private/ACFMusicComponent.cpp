// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFMusicComponent.h"
#include <Game/ACFFunctionLibrary.h>
#include <Game/ACFGameState.h>
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>
#include <Sound/SoundCue.h>

// Sets default values for this component's properties
UACFMusicComponent::UACFMusicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


void UACFMusicComponent::SetMusicCueByState(USoundCue* musicCue, EBattleState battleState)
{
	MusicCueByState.Add(battleState, musicCue);
}

void UACFMusicComponent::PlayMusicCueByState(EBattleState battleState)
{
	USoundCue* musicToPlay = GetMusicCueByState(battleState);
	if (musicToPlay) {
		UAudioComponent* audioComp = UGameplayStatics::SpawnSound2D(this, musicToPlay,VolumeMult, 1.0f,0.f);
		if (audioComp) {
			audioComp->FadeIn(FadeTime, VolumeMult);
			if (currentComponent) {
				currentComponent->FadeOut(FadeTime, 0.f);
			}
			currentComponent = audioComp;
			currentlyPlayingMusic = musicToPlay;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Missing music cue! - UACFMusicComponent "));
	}
}

class USoundCue* UACFMusicComponent::GetMusicCueByState(EBattleState battleState) const
{
	if (MusicCueByState.Contains(battleState)) {
		return *MusicCueByState.Find(battleState);
	}
	return nullptr;
}

// Called when the game starts
void UACFMusicComponent::BeginPlay()
{
	Super::BeginPlay();

	AACFGameState* gameState = UACFFunctionLibrary::GetACFGameState(this);
	if (gameState) {
		gameState->OnBattleStateChanged.AddDynamic(this, &UACFMusicComponent::HandleStateChanged);
		if (bAutoStart) {
			PlayMusicCueByState(gameState->GetBattleState());
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Missing ACFGameState! - UACFMusicComponent "));
	}
}



void UACFMusicComponent::HandleStateChanged(const EBattleState& battleState)
{
	PlayMusicCueByState(battleState);
}

