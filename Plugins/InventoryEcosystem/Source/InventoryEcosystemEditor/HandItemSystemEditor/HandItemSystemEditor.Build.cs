// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HandItemSystemEditor : ModuleRules
{
	public HandItemSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Core", 
				"CoreUObject", 
				"Engine", 
				"AnimationModifiers", 
				"AnimationBlueprintLibrary",
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
                "GameplayAbilities", 


                "InventoryCore",
                "ItemSystem",
                "InteractSystem",
                "HandItemSystem",
                "InventoryTools",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "ApplicationCore",
                "Json",
                "JsonUtilities",

                "Engine",
                "GameplayTags",
                "UMG",
				"InputCore",
				"WorkspaceMenuStructure",
				"Projects", 
				"DeveloperSettings",
				"GameplayAbilities",
				"GameplayTasks",
				// ... add private dependencies that you statically link with here ...	
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
