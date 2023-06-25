// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2022. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ACFSetFeetIKEnabledNotify.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERCONTROLLER_API UACFSetFeetIKEnabledNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bEnabled = true;
	
};
