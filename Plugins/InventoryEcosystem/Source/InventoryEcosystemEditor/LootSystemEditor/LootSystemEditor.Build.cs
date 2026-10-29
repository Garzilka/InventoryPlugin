// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LootSystemEditor : ModuleRules
{
	public LootSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"BlueprintGraph",
				"Kismet",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"InputCore",
				"CoreUObject",
				
				"Slate",
				"SlateCore",
				
				"GameplayTags",
				"GameplayTagsEditor", 
				
				"UMG",
				"Projects", 
				
				
				"InteractSystem", 
				"LootSystem", 
				"InventoryCore",
				"ItemSystem",
				"InventoryCoreEditor",
				
			}
			);
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("EditorStyle");
			PrivateDependencyModuleNames.Add("GraphEditor");
			PrivateDependencyModuleNames.Add("PropertyEditor");
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
