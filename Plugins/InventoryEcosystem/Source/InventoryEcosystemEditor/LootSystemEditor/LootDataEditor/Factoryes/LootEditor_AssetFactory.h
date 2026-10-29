#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootSystemEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Factories/Factory.h"
#include "LootEditor_AssetFactory.generated.h"

UCLASS()
class LOOTSYSTEMEDITOR_API ULootEditor_AssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	ULootEditor_AssetFactory()
	{
		bCreateNew = true;
		bEditAfterNew = true;

		SupportedClass = ULootData::StaticClass();
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
	{
		ULootData* LootEditor = NewObject<ULootData>(InParent, InClass, InName, Flags | RF_Transactional);
		return LootEditor;
	}

	virtual uint32 GetMenuCategories() const override       { return FLootSystemEditorModule::InventoryEcosystemCategory; };
	virtual FText GetDisplayName() const override           { return  FText::FromString(TEXT("Loot Editor")); };
	virtual FString GetDefaultNewAssetName() const override { return FString(TEXT("New Loot Editor")); };
};


class FAssetTypeActions_LootEditor_AssetAction : public FAssetTypeActions_Base
{

public:

	virtual FText GetName() const override { return FText::FromString(TEXT("Loot Editor")); };
	virtual FColor GetTypeColor() const override { return FColor::FromHex("#00AFFF"); }
	virtual UClass* GetSupportedClass() const override { return ULootData::StaticClass(); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override
	{
		const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

		for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
		{
			if (ULootData* LootData = Cast<ULootData>(*ObjIt))
			{
				TSharedRef<FLootEditorAssetEditor> NewLootEditor(new FLootEditorAssetEditor());
				NewLootEditor->InitLootAssetEditor(Mode, EditWithinLevelEditor, LootData);
				
			}
		}
	}

	virtual uint32 GetCategories() override { return FLootSystemEditorModule::InventoryEcosystemCategory; };
};