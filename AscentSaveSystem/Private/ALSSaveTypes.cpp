// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ALSSaveTypes.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


bool FALSBaseData::Serialize(FArchive& Ar)
{
	/*Super::Serialize(Ar);*/
	Ar << alsName;
	return true;
}

FALSObjectData::FALSObjectData(const UObject* Object) : Super()
{		
	alsName = Object->GetFName();	
	Class = Object->GetClass();
}

bool FALSObjectData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << Class;
	Ar << alsName;
	Ar << Data;
	return true;
}

bool FALSComponentData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar << Transform;
	return true;
}

bool FALSActorData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.SerializeBits(&bHiddenInGame, 1);

	Ar << Transform;
	Ar << ComponentRecords;
	return true;
}

bool FALSLevelData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << Actors;

	return true;
}

