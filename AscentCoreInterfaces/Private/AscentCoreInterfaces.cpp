// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "AscentCoreInterfaces.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FAscentCoreInterfaces"

void FAscentCoreInterfaces::StartupModule()
{
}

void FAscentCoreInterfaces::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAscentCoreInterfaces, AscentCoreInterfaces);