// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFActionsFunctionLibrary.h"
#include "GameplayTagsManager.h"
#include "ACFActionsDeveloperSettings.h"


bool UACFActionsFunctionLibrary::IsValidMovesetActionsTag(FGameplayTag TagToCheck)
{
	const FGameplayTag root = GetMovesetActionsTagRoot();
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

FGameplayTag UACFActionsFunctionLibrary::GetDefaultActionsState()
{
	UACFActionsDeveloperSettings* settings = GetMutableDefault<UACFActionsDeveloperSettings>();
	
	if (settings) {
		return settings->DefaultActionsState;
	}

	return FGameplayTag();
}
FGameplayTag UACFActionsFunctionLibrary::GetMovesetActionsTagRoot()
{
	UACFActionsDeveloperSettings* settings = GetMutableDefault<UACFActionsDeveloperSettings>();

	if (settings) {
		return settings->MovesetActionsRootTag;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Moveset Actions Root Tag! - UACFActionsFunctionLibrary "));

	return FGameplayTag();
}
