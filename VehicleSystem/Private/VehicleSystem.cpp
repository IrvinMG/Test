// Copyright (C) Pakyman Prod. 2020. All Rights Reserved.

#include "VehicleSystem.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FVehicleSystem"

void FVehicleSystem::StartupModule()
{
}

void FVehicleSystem::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVehicleSystem, VehicleSystem);