// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Animation/AnimSequence.h"
#include "ACFCoreTypes.h"
#include "ACFCCTypes.h"
#include "ACFAnimTypes.generated.h"

/**
 * 
 */



USTRUCT(BlueprintType)
struct FMoveset : public FACFStruct {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TSubclassOf<class UACFMovesetLayer> Moveset;
};


USTRUCT(BlueprintType)
struct FOverlayLayer : public FACFStruct {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TSubclassOf<class UACFOverlayLayer> Overlay;
};


USTRUCT(BlueprintType)
struct FRiderLayer : public FACFStruct {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TSubclassOf<class UACFRiderLayer> RiderLayer;
};




