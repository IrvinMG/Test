// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "UI/ACFHUD.h"
#include <Kismet/GameplayStatics.h>
#include "Game/ACFPlayerController.h"
#include "UI/ACFHUDUserWidget.h"

void AACFHUD::BeginPlay()
{	
	Super::BeginPlay();
}

void AACFHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	AACFPlayerController* playerController = Cast< AACFPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if ( playerController)
	{
		playerController->OnPossessedCharacterChanged.RemoveDynamic(this, &AACFHUD::HandleCharacterPossessedChanged);
	}
}

void AACFHUD::PostInitProperties()
{
	Super::PostInitProperties();
	InitHUD();
}

void AACFHUD::InitHUD()
{
	AACFPlayerController* playerController = Cast< AACFPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (HUDClass && playerController)
	{
 		HUDWidget = CreateWidget<UACFHUDUserWidget>(GetWorld(), HUDClass);
		HUDWidget->AddToViewport(0);

		playerController->OnPossessedCharacterChanged.AddDynamic(this, &AACFHUD::HandleCharacterPossessedChanged);
	}

}

void AACFHUD::EnableCrosshair(bool bEnable)
{
	if (HUDWidget)
		HUDWidget->EnableCrosshair(bEnable);
}

void AACFHUD::SetHudEnabled(bool bEnabled)
{
	if (!HUDWidget)
		return;

	if (bEnabled)
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	else
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
}


void AACFHUD::HandleCharacterPossessedChanged(const class AACFCharacter* newChar)
{
	if (HUDWidget)
		HUDWidget->OnPossessedCharacterChanged(newChar);
}
