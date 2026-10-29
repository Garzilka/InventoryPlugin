#pragma once
#include "CoreMinimal.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Buttons/ListAddButton/W_ListAddButton.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;


class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootSettingsList : public FWorkflowTabFactory
{
public:

	FLootEditorTabFactory_LootSettingsList(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);

	void OnButtonClicked(SW_ListAddButton* Sw_ListAddButton) const;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;


	void RebuildList() const;
private:
	TSharedPtr<SVerticalBox> Place;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};