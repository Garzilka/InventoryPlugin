#include "W_LootPresetMainWidget.h"

#include "ISinglePropertyView.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "W_LootPreset.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/MetaData/W_LootMetaData.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"


void SW_LootPresetMainWidget::Construct(const FArguments& InArgs)
{
	LootObjectGUID = InArgs._LootObjectGUID;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	
	UpdateHandleMap();
	
	this->ChildSlot
	.Padding(FMargin(0))
	[
		SNew(SBox)
		.Padding(10.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0.f)
			.AutoHeight()
			[
				SAssignNew(B_MetaPlace, SVerticalBox)
			]
			+ SVerticalBox::Slot()
			.Padding(0.f, 10.f, 0.f, 0.f)
			.AutoHeight()
			[
				SAssignNew(VB_LootData, SVerticalBox)
			]
		]
	];
	
	if (LootObjectGUID.IsValid() && !LootObjectGUID.IsNone())
	{
		Init(LootObjectGUID);
	}
	else
	{
		SetDefault();
	}
}

void SW_LootPresetMainWidget::Init(FName NewObjectGUID)
{
	if (NewObjectGUID == LootObjectGUID) return;
	LootObjectGUID = NewObjectGUID;
	
	UpdateHandleMap();	
	uint32 childrenCount = 0;
	PropertyHandle_HandleMap->AsMap()->GetNumElements(childrenCount);
	
	PropertyHandle_PresetStruct = nullptr;
	PropertyHandle_MetaData = nullptr;
	
	for (uint32 Idx = 0; Idx < childrenCount; ++Idx)
	{
		TSharedRef<IPropertyHandle> LPropertyHandle_Pair = PropertyHandle_HandleMap->AsMap()->GetElement(Idx);
	
		FName KeyName;
		LPropertyHandle_Pair->GetKeyHandle()->GetValue(KeyName);		
		if (KeyName != LootObjectGUID) continue;
	
		PropertyHandle_PresetStruct			= LPropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, GenerateSettings));
		PropertyHandle_MetaData			= LPropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, MetaData));
		break;
	}
	GenerateLootData();
}

void SW_LootPresetMainWidget::SetDefault()
{
	if (VB_LootData.IsValid())
	{
		VB_LootData->ClearChildren();
	}
	
	if (B_MetaPlace.IsValid())
	{
		B_MetaPlace->ClearChildren();
	}	
}


void SW_LootPresetMainWidget::UpdateHandleMap()
{
	if (LootAssetEditor.Pin() && LootAssetEditor.Pin()->GetPressetDataAssetProperty())
	{
		PropertyHandle_HandleMap = LootAssetEditor.Pin()->GetPressetDataAssetProperty()->GetPropertyHandle();
	}
}

void SW_LootPresetMainWidget::GenerateLootData()
{
	if (!VB_LootData.IsValid())
	{
		VB_LootData.Reset();
		VB_LootData = SNew(SVerticalBox);
	}
	else
	{
		VB_LootData->ClearChildren();
	}
	if (PropertyHandle_PresetStruct && PropertyHandle_PresetStruct.IsValid())
	{
		VB_LootData->AddSlot()
		.AutoHeight()
		[
			SNew(SW_LootPreset)
			.LootAssetEditorRef(LootAssetEditor)
			.PropertyHandle_Struct(PropertyHandle_PresetStruct)
		];
	}
	
	if (!B_MetaPlace.IsValid())
	{
		B_MetaPlace.Reset();
		B_MetaPlace = SNew(SVerticalBox);
	}
	else
	{
		B_MetaPlace->ClearChildren();
	}
	if (PropertyHandle_PresetStruct && PropertyHandle_PresetStruct.IsValid())
	{
		B_MetaPlace->AddSlot()
		[
			SNew(SW_LootMetaData)
			.LootAssetEditor(LootAssetEditor)
			.LootObjectGUID(LootObjectGUID)
			.PropertyHandle_Struct(PropertyHandle_MetaData)
			.ObjectType(ECategorySelector::Preset)
		];
	}
}
