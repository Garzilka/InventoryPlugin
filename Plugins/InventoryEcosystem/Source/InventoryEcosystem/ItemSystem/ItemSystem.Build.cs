// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ItemSystem : ModuleRules
{
	public ItemSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"DeveloperSettings",

				"InventoryCore",
				"InteractSystem",
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
			);

		// Редакторные классы (GEditor в PostEditChangeProperty) требуют UnrealEd.
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
