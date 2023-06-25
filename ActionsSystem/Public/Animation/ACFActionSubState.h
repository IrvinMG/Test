// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ACFActionSubState.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSSYSTEM_API UACFActionSubState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected: 


	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)  override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)  override;
	
public:

//  	UPROPERTY(EditAnywhere)
//  	bool bActivateOnlyOnServer = true;
	
};
