// Copyright (C) Pakyman Prod. 2020. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VehicleSystem : ModuleRules
{
	public VehicleSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
                Path.Combine(ModuleDirectory, "Public")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] 
			{
				
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AscentCombatFramework",
				"AIModule",
				"AscentCoreInterfaces",
				"AIFramework",
                "ChaosVehicles"

            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AdvancedRPGSystem",
				"CollisionsManager",
                "AscentCoreInterfaces",
                "AIFramework",
                "ChaosVehicles",
				"MountSystem"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			}
			);
	}
}
