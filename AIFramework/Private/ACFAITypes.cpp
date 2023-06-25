// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFAITypes.h"
#include "ACFAIController.h"
#include "Actors/ACFCharacter.h"

 FAIAgentsInfo::FAIAgentsInfo(AACFCharacter* inChar)
{
    AICharacter = inChar;
    if (inChar) { 
        characterClass = inChar->GetClass();
        AIController = Cast<AACFAIController>(inChar->GetController());
    }
}

FAIAgentsInfo::FAIAgentsInfo(AACFCharacter* inChar, AACFAIController* inContr)
{
    AICharacter = inChar;
    AIController = inContr;
    characterClass = inChar->GetClass();
}

