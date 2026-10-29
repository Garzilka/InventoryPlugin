// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventorySystem : ModuleRules
{
	public InventorySystem(ReadOnlyTargetRules Target) : base(Target)
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
                "CoreUObject",
				"Engine", 
				"InputCore", 
				"Slate",
				"Projects", 
                "DeveloperSettings",


                "InventoryCore",
                "ItemSystem",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
				"InventoryTools",
				"SlateCore",
                "ToolMenus",
                "ApplicationCore",
                "Json",
                "JsonUtilities",

                "Engine",
                "Slate",
                "GameplayTags",
                "UMG",
                "Slate",
				"InputCore",
				"Projects", 
				"DeveloperSettings"	
			}
			);
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("EditorStyle");
			PrivateDependencyModuleNames.Add("LevelEditor");
			PrivateDependencyModuleNames.Add("AssetTools");
			PrivateDependencyModuleNames.Add("KismetWidgets");
			PrivateDependencyModuleNames.Add("GraphEditor");
			PrivateDependencyModuleNames.Add("AnimGraph");
			PrivateDependencyModuleNames.Add("PropertyEditor");
			PrivateDependencyModuleNames.Add("WorkspaceMenuStructure");
		}
        PrivateIncludePathModuleNames.AddRange(new string[] 
		{ 
			"MainFrame",
			"Settings"
		});


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
