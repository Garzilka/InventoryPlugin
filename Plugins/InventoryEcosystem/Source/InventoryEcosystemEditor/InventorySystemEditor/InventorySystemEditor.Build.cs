// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventorySystemEditor : ModuleRules
{
	public InventorySystemEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"WorkspaceMenuStructure", 
				"Projects", 
                "DeveloperSettings",
                "BlueprintGraph",


                "InventoryCore",
                "ItemSystem",
                "InteractSystem",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "PropertyEditor",
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
				"WorkspaceMenuStructure",
				"Projects",
				"DeveloperSettings"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"EditorStyle",
					"LevelEditor",
					"AssetTools",
					"KismetWidgets",
					"GraphEditor",
					"AnimGraph",
					// ... add private dependencies that you statically link with here ...	
				});
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
