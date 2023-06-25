// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/ACFTypes.h"
#include "ACFMusicComponent.generated.h"


UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class MUSICMANAGER_API UACFMusicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFMusicComponent();

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetMusicCueByState(USoundCue* musicCue, EBattleState battleState);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void PlayMusicCueByState(EBattleState battleState);

	UFUNCTION(BlueprintCallable, Category = ACF)
	class USoundCue* GetMusicCueByState(EBattleState battleState) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	class USoundCue* GetCurrntlyPlayingMusic() const {
		return currentlyPlayingMusic;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
	bool bAutoStart = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
	TMap<EBattleState, USoundCue*> MusicCueByState;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
	float FadeTime = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
	float VolumeMult = 1.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = ACF)
	class USoundConcurrency* concurrencySettings;
private: 

	UFUNCTION()
	void HandleStateChanged(const EBattleState& battleState);

	UPROPERTY()
	class USoundCue*currentlyPlayingMusic;

	UPROPERTY()
	class UAudioComponent* currentComponent;
		
};
