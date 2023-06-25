// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#include "ARSFunctionLibrary.h"
#include "ARSDeveloperSettings.h"
#include "ARSGenerationRulesDataAsset.h"
#include "ARSTypes.h"
#include <GameplayTagsManager.h>

bool UARSFunctionLibrary::TryGetGenerationRuleByPrimaryAttributeType(const FGameplayTag& PrimaryAttributeTag, FGenerationRule& outRule)
{

    const UARSGenerationRulesDataAsset* rulesDT = GetGenerationRulesData();
    if (rulesDT) {
        return rulesDT->TryGetGenerationRulesForPrimaryAttributes(PrimaryAttributeTag, outRule);
    }

    UE_LOG(LogTemp, Error, TEXT("Missing Generation Rule! - ARSFUNCTION LIBRARY"));

    return false;
}

class UARSGenerationRulesDataAsset* UARSFunctionLibrary::GetGenerationRulesData()
{
    UARSDeveloperSettings* settings = GetMutableDefault<UARSDeveloperSettings>();

    if (settings) {
        return settings->GetAttributesGenerationRules();
    }
    return nullptr;
}

FGameplayTag UARSFunctionLibrary::GetAttributesTagRoot()
{
    UARSDeveloperSettings* settings = GetMutableDefault<UARSDeveloperSettings>();

    if (settings) {
        return settings->PrimaryAttributesRootTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
    return FGameplayTag();
}

FGameplayTag UARSFunctionLibrary::GetParametersTagRoot()
{
    UARSDeveloperSettings* settings = GetMutableDefault<UARSDeveloperSettings>();

    if (settings) {
        return settings->AttributesRootTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
    return FGameplayTag();
}

FGameplayTag UARSFunctionLibrary::GetStatisticsTagRoot()
{
    UARSDeveloperSettings* settings = GetMutableDefault<UARSDeveloperSettings>();

    if (settings) {
        return settings->StatisticsRootTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
    return FGameplayTag();
}

bool UARSFunctionLibrary::TryGetAttributeFromArray(FGameplayTag attributeTag, 
    const TArray<FAttribute>& attributesArray, FAttribute& outAttribute)
{
    if (attributesArray.Contains(attributeTag)) {
        outAttribute = *attributesArray.FindByKey(attributeTag);
        return true;
    }
    return false;
}

bool UARSFunctionLibrary::TryGetStatisticFromArray(FGameplayTag statTag, 
    const TArray<FStatistic>& statsArray, FStatistic& outStat)
{
    if (statsArray.Contains(statTag)) {
        outStat = *statsArray.FindByKey(statTag);
        return true;
    }
    return false;
}

uint8 UARSFunctionLibrary::GetMaxLevel()
{
    UARSDeveloperSettings* settings = GetMutableDefault<UARSDeveloperSettings>();

    if (settings) {
        return settings->MaxLevel;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing MaxLevel! - ARSFUNCTION LIBRARY"));

    return 0;
}

bool UARSFunctionLibrary::IsValidStatisticTag(FGameplayTag TagToCheck)
{
    const FGameplayTag root = GetStatisticsTagRoot();

    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UARSFunctionLibrary::IsValidAttributeTag(FGameplayTag TagToCheck)
{
    const FGameplayTag root = GetAttributesTagRoot();

    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UARSFunctionLibrary::IsValidParameterTag(FGameplayTag TagToCheck)
{
    const FGameplayTag root = GetParametersTagRoot();

    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}
