// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ALSFunctionLibrary.h"
#include "HighResScreenshot.h"
#include "ALSSavableInterface.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ALSSaveGameSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ALSLoadAndSaveSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "ALSSaveTypes.h"
#include "Misc/Paths.h"
#include "TextureResource.h"
#include "Serialization/BulkData.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "GameFramework/Pawn.h"


bool UALSFunctionLibrary::TrySaveSceenshot(const FString& fileName, const int32 width /*= 640*/, const int32 height/*= 480*/)
{
	if (!GEngine || !GEngine->GameViewport)	{
		return false;
	}

	if (auto* Viewport = GEngine->GameViewport->Viewport)	{
		const FString screenshotPath = ConstructScreenshotPath(fileName);

		FHighResScreenshotConfig& screenshotConfig = GetHighResScreenshotConfig();
		screenshotConfig.SetHDRCapture(true);
		screenshotConfig.FilenameOverride = screenshotPath;
		GScreenshotResolutionX = width;
		GScreenshotResolutionY = height;
		Viewport->TakeHighResScreenShot();
		return true;
	}
	return false;
}

FString UALSFunctionLibrary::ConstructScreenshotPath(const FString& fileName)
{
	return FPaths::ProjectSavedDir() + fileName + ".png";
}

UTexture2D* UALSFunctionLibrary::GetScreenshotByName(const FString& fileName)
{
	UTexture2D* Texture{ nullptr };
	const FString path = ConstructScreenshotPath(fileName);
	TArray<uint8> RawFileData;
	if (GEngine && FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		IImageWrapperModule& imgWrapperModule = FModuleManager::LoadModuleChecked < IImageWrapperModule >(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> imgWrapper = imgWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		if (imgWrapper.IsValid() && imgWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			TArray64<uint8> UncompressedBGRA;
			if (imgWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				Texture = UTexture2D::CreateTransient(imgWrapper->GetWidth(), imgWrapper->GetHeight(), PF_B8G8R8A8);
				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

bool UALSFunctionLibrary::ShouldSaveActor(AActor* actor)
{
	if (actor->IsValidLowLevel()) {
		return (actor->GetClass()->ImplementsInterface(UALSSavableInterface::StaticClass()) &&
			!IALSSavableInterface::Execute_ShouldBeIgnored(actor));
	}
	return false;
}


bool UALSFunctionLibrary::IsNewGame(const UObject* WorldContextObject)
{
	return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UALSLoadAndSaveSubsystem>()->IsNewGame();
}

bool UALSFunctionLibrary::IsSpecialActor(const UObject* WorldContextObject, const AActor* actor)
{
	const APlayerController* pc = Cast<APlayerController>(actor);
	if (pc) {
		return true;
	}
	const APawn* pawn = Cast<APawn>(actor);
	if (pawn) {
		return pawn->IsPlayerControlled();
	}
	return false;
}

void UALSFunctionLibrary::ExecuteFunctionsOnSavableComponents(const AActor* actorOwner, const FName& functionName)
{
	TArray<UActorComponent*> Components = IALSSavableInterface::Execute_GetComponentsToSave(actorOwner);
	for (auto component : Components) {

		UFunction* func = component->FindFunction(functionName);
		if (func) {
			component->ProcessEvent(func, nullptr);
		}
	}
}

void UALSFunctionLibrary::DeserializeActor(AActor* Actor, const FALSActorData& Record)
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	Actor->Tags = Record.Tags;

	Actor->SetActorHiddenInGame(Record.bHiddenInGame);
	UALSFunctionLibrary::DeserializeActorComponents(Actor, Record);

	FMemoryReader MemoryReader(Record.Data, true);
	FALSSaveGameArchive Archive(MemoryReader, false);
	Actor->Serialize(Archive);

}

void UALSFunctionLibrary::DeserializeActorComponents(AActor* Actor, const FALSActorData& ActorRecord)
{
	TArray<UActorComponent*>Components = IALSSavableInterface::Execute_GetComponentsToSave(Actor);
	for (auto* Component : Components) {

		const FALSComponentData* Record = ActorRecord.GetComponentData(Component);

		if (!Record) {
			continue;
		}

		USceneComponent* Scene = Cast<USceneComponent>(Component);
		if (Scene && Scene->Mobility == EComponentMobility::Movable) {
			Scene->SetRelativeTransform(Record->Transform);
		}

		FMemoryReader MemoryReader(Record->Data, true);
		FALSSaveGameArchive Archive(MemoryReader, false);
		Component->Serialize(Archive);
	}
}

FALSActorData UALSFunctionLibrary::SerializeActor(AActor* actor)
{
	const UALSSaveGameSettings* saveSettings = GetMutableDefault<UALSSaveGameSettings>();
	IALSSavableInterface::Execute_OnSaved(actor);
	UALSFunctionLibrary::ExecuteFunctionsOnSavableComponents(actor, saveSettings->GetOnComponentSavedFunctionName());
	FALSActorData Record = { actor };

	Record.bHiddenInGame = actor->IsHidden();
	Record.Transform = actor->GetTransform();
	Record.Tags = actor->Tags;

	UALSFunctionLibrary::SerializeComponents(actor, Record);

	FMemoryWriter MemoryWriter(Record.Data, true);
	FALSSaveGameArchive Archive(MemoryWriter, false);
	actor->Serialize(Archive);

	return Record;
}

void UALSFunctionLibrary::SerializeComponents(const AActor* Actor, FALSActorData& ActorRecord) {
	TArray<UActorComponent*>Components = IALSSavableInterface::Execute_GetComponentsToSave(Actor);
	for (auto* Component : Components) {
		FALSComponentData ComponentRecord;
		ComponentRecord.alsName = Component->GetFName();
		ComponentRecord.Class = Component->GetClass();
		const USceneComponent* Scene = Cast<USceneComponent>(Component);
		if (Scene && Scene->Mobility == EComponentMobility::Movable) {
			ComponentRecord.Transform = Scene->GetRelativeTransform();
		}
		FMemoryWriter MemoryWriter(ComponentRecord.Data, true);
		FALSSaveGameArchive Archive(MemoryWriter, false);
		Component->Serialize(Archive);
		ActorRecord.AddComponentData(ComponentRecord);
	}

}