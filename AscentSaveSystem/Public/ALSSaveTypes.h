// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/LevelStreaming.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "GameFramework/Actor.h"
#include "Engine/Level.h"
#include "Components/ActorComponent.h"
#include "ALSSaveTypes.generated.h"

/**
 * 
 */

struct FALSSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FALSSaveGameArchive(FArchive& InInnerArchive, bool isSavegame)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
	{
		ArIsSaveGame = true;
	}
};




USTRUCT()
struct FALSBaseData
{
	GENERATED_BODY()

public: 

	UPROPERTY(SaveGame)
	FName alsName;

	FName GetName() const {
		return alsName;
	}

	FALSBaseData() : alsName() {}

	virtual bool Serialize(FArchive& Ar);
	friend FArchive& operator<<(FArchive& Ar, FALSBaseData& Record)
	{
		Record.Serialize(Ar);
		return Ar;
	}
	virtual ~FALSBaseData() {}

};




/** Represents a serialized Object */
USTRUCT()
struct FALSObjectData : public FALSBaseData
{
	GENERATED_BODY()

public:

	UPROPERTY(SaveGame)
	UClass* Class;

	UPROPERTY(SaveGame)
	TArray<uint8> Data;

	FALSObjectData() : Super(), Class(nullptr) {}
	FALSObjectData(const UObject* Object);

	virtual bool Serialize(FArchive& Ar) override;

	bool IsValid() const
	{
		return !alsName.IsNone() && Class && Data.Num() > 0;
	}

	FORCEINLINE bool operator== (const FALSObjectData& Other) const
	{
		return this->alsName == Other.alsName;
	}
	FORCEINLINE bool operator!= (const FALSObjectData& Other) const
	{
		return this->alsName != Other.alsName;
	}
	FORCEINLINE bool operator== (const UObject* Other) const
	{
		return this->alsName == Other->GetFName();
	}

	FORCEINLINE bool operator!= (const UObject* Other) const
	{
		return this->alsName != Other->GetFName();
	}

	FORCEINLINE bool operator== (const FName Name) const
	{
		return this->alsName == Name;
	}

	FORCEINLINE bool operator!= (const FName Name) const
	{
		return this->alsName != Name;
	}

};


/** Represents a serialized Component */
USTRUCT()
struct FALSComponentData : public FALSObjectData
{
	GENERATED_BODY()

public: 
	UPROPERTY(SaveGame)
	FTransform Transform;

	FALSComponentData() : Super() {}
	FALSComponentData(const UActorComponent* comp) : Super(comp) {}

	virtual bool Serialize(FArchive& Ar) override;

	FORCEINLINE bool operator== (const UActorComponent* Other) const
	{
		return this->alsName == Other->GetFName();
	}

	FORCEINLINE bool operator!= (const UActorComponent* Other) const
	{	
		return this->alsName != Other->GetFName();
	}

	FORCEINLINE bool operator== (const FName Name) const
	{
		return this->alsName == Name;
	}

	FORCEINLINE bool operator!= (const FName Name) const
	{
		return this->alsName != Name;
	}
};


/** Represents a serialized Actor */
USTRUCT()
struct FALSActorData : public FALSObjectData
{
	GENERATED_BODY()

private:
	UPROPERTY(SaveGame)
	TArray<FALSComponentData> ComponentRecords;


public:

	FALSActorData()
        : Super()  {
        bHiddenInGame = false;
	}

	UPROPERTY(SaveGame)
	TArray<FName> Tags;

	const FALSComponentData* GetComponentData(const UActorComponent* component) const {
		return ComponentRecords.FindByKey(component->GetFName());
	}

	bool HasComponent(const UActorComponent* component) const {
		return ComponentRecords.Contains(component->GetFName());
	}

	void AddComponentData(const FALSComponentData& component)  {
		 ComponentRecords.Add(component);
	}
	UPROPERTY(SaveGame)
	bool bHiddenInGame;

	UPROPERTY(SaveGame)
	FTransform Transform;

	FALSActorData(const AActor* Actor) : Super(Actor) {}

	virtual bool Serialize(FArchive& Ar) override;

	FORCEINLINE bool operator== (const FALSActorData& Other) const
	{
		return this->alsName == Other.GetName();
	}

	FORCEINLINE bool operator!= (const FALSActorData& Other) const
	{
		return this->alsName != Other.GetName();
	}

	FORCEINLINE bool operator== (const AActor* Other) const
	{
		return this->alsName == Other->GetFName();
	}

	FORCEINLINE bool operator!= (const AActor* Other) const
	{
		return this->alsName != Other->GetFName();
	}

	FORCEINLINE bool operator== (const FName Name) const
	{
		return this->alsName == Name;
	}

	FORCEINLINE bool operator!= (const FName Name) const
	{
		return this->alsName != Name;
	}
};

/** Represents a level in the world (streaming or persistent) */
USTRUCT()
struct FALSLevelData : public FALSObjectData
{
	GENERATED_BODY()

protected:

	/** Records of the World Actors */
	UPROPERTY(SaveGame)
	TArray<FALSActorData> Actors;
public:

	void AddActorRecord(const FALSActorData& actorData) {
		Actors.Add(actorData);
	}

	TArray<FALSActorData> GetActorsCopy() const {
		return Actors;
	}

	const FALSActorData* GetActorData(const AActor* actor) const {
		return Actors.FindByKey(actor);
	}	

	bool HasActor(const AActor* actor) const {
		return Actors.Contains(actor);
	}

	FALSLevelData() {};
	FALSLevelData(const ULevel* level) : Super(level) {}
	virtual bool Serialize(FArchive& Ar) override;

	bool IsValid() const { return !alsName.IsNone(); }
};

USTRUCT()
struct FALSActorLoaded 
{
	GENERATED_BODY()

public:
	FALSActorLoaded(const FTransform& inTransform) {
		transform = inTransform;
	}

	FALSActorLoaded() {};
	FTransform transform;
};

USTRUCT()
struct FALSPlayerData
{
	GENERATED_BODY()

public:

	FALSPlayerData(){};

	FALSPlayerData(const FALSActorData& inController, const FALSActorData& inPawn) {
		PlayerController = inController;
		Pawn = inPawn;
	}

	UPROPERTY(SaveGame)
	FALSActorData Pawn;

	UPROPERTY(SaveGame)
	FALSActorData PlayerController;

};

UCLASS()
class ASCENTSAVESYSTEM_API UALSSaveTypes : public UObject
{
	GENERATED_BODY()
	
};
