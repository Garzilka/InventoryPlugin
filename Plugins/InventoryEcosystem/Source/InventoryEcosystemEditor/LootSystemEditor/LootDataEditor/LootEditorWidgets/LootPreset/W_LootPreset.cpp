#include "W_LootPreset.h"

#include "PropertyHandle.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_StandardizedRow.h"
#include "RespawnData/W_RespawnData.h"
#include "SettingBlock/W_SettingBlock.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<SW_BaseArrayElement> SW_AC_LootPreset::CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx,
	TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
		return SNew(SW_SettingBlock)
			.LootAssetEditorRef(InLootAssetEditor)
			.PropertyHandle_Struct(InPropertyHandle_Array->AsArray()->GetElement(Idx))
			.ElementIdx(Idx);
}

void SW_LootPreset::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;

	if (!(PropertyHandle_Struct && PropertyHandle_Struct.IsValid())) return;
	
	PropertyHandle_bUseRespawnView	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettings, bUseRespawnView));
	if (!PropertyHandle_bUseRespawnView.IsValid()) return;

	PropertyHandle_bIsUniqueLoot	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettings, bIsUniqueLoot));
	if (!PropertyHandle_bIsUniqueLoot.IsValid()) return;

	PropertyHandle_RespawnSettings	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettings, RespawnSettings));
	if (!PropertyHandle_RespawnSettings.IsValid()) return;

	PropertyHandle_GenerateBlocks	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettings, GenerateBlocks));
	if (!PropertyHandle_GenerateBlocks.IsValid()) return;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SW_StandardizedRow)
			.DisplayName(PropertyHandle_bIsUniqueLoot->GetPropertyDisplayName())
			.PropertyHandle(PropertyHandle_bIsUniqueLoot)
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 8.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SW_StandardizedRow)
			.DisplayName(PropertyHandle_bUseRespawnView->GetPropertyDisplayName())
			.PropertyHandle(PropertyHandle_bUseRespawnView)
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 5.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SBox)
			.Visibility(this, &SW_LootPreset::OnGetRespawnDataVisibility)
			[
				SNew(SW_RespawnData)
				.LootAssetEditorRef(LootAssetEditor)
				.PropertyHandle_Struct(PropertyHandle_RespawnSettings)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 5.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SW_AC_LootPreset)
			.DisplayName(PropertyHandle_GenerateBlocks->GetPropertyDisplayName())
			.ArrayRef(PropertyHandle_GenerateBlocks)
			.LootAssetEditorRef(LootAssetEditor)
		]
		
	];
}

EVisibility SW_LootPreset::OnGetRespawnDataVisibility() const
{
	if (!(PropertyHandle_bUseRespawnView && PropertyHandle_bUseRespawnView.IsValid())) return EVisibility::Collapsed;
	bool LIsView;
	PropertyHandle_bUseRespawnView->GetValue(LIsView);
	return (LIsView ? EVisibility::Visible : EVisibility::Collapsed);
}


