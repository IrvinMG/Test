// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "CombinedAnimationsSystem.h"

#define LOCTEXT_NAMESPACE "FCombinedAnimationsSystemModule"

void FCombinedAnimationsSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCombinedAnimationsSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCombinedAnimationsSystemModule, CombinedAnimationsSystem)