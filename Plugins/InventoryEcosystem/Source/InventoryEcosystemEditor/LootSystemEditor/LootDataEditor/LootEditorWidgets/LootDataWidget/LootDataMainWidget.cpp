#include "LootDataMainWidget.h"

#include "ISinglePropertyView.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/MetaData/W_LootMetaData.h"
#include "Modules/ModuleManager.h"
#include "RandomGeneration/W_RandomGenerationData.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"


void SLootDataMainWidget::Construct(const FArguments& InArgs)
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
			.Padding(0.f, 10.f, 0.f, 0.f)
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

void SLootDataMainWidget::Init(FName NewObjectGUID)
{
	if (NewObjectGUID == LootObjectGUID) return;
	LootObjectGUID = NewObjectGUID;

	
	UpdateHandleMap();	
	uint32 childrenCount = 0;
	PropertyHandle_HandleMap->AsMap()->GetNumElements(childrenCount);

	for (uint32 Idx = 0; Idx < childrenCount; ++Idx)
	{
		TSharedRef<IPropertyHandle> LPropertyHandle_Pair = PropertyHandle_HandleMap->AsMap()->GetElement(Idx);
	
		FName KeyName;
		LPropertyHandle_Pair->GetKeyHandle()->GetValue(KeyName);		
		if (KeyName != LootObjectGUID) continue;
	
		PropertyHandle_RandomGeneration		= LPropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootRowData, RandomGeneration));
		PropertyHandle_MetaData			= LPropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, MetaData));
		break;
	}
	GenerateLootData();
}

void SLootDataMainWidget::SetDefault()
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


void SLootDataMainWidget::UpdateHandleMap()
{
	if (LootAssetEditor.Pin() && LootAssetEditor.Pin()->GetLootDataAssetProperty())
	{
		PropertyHandle_HandleMap = LootAssetEditor.Pin()->GetLootDataAssetProperty()->GetPropertyHandle();
	}
}

void SLootDataMainWidget::GenerateLootData()
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
	
	VB_LootData->AddSlot()
	.AutoHeight()
	[
		SNew(SW_RandomGenerationData)
		.LootAssetEditorRef(LootAssetEditor)
		.PropertyHandle_Struct(PropertyHandle_RandomGeneration)
	];
	
	if (!B_MetaPlace.IsValid())
	{
		B_MetaPlace.Reset();
		B_MetaPlace = SNew(SVerticalBox);
	}
	else
	{
		B_MetaPlace->ClearChildren();
	}

	B_MetaPlace->AddSlot()
	[
		SNew(SW_LootMetaData)
		.LootAssetEditor(LootAssetEditor)
		.LootObjectGUID(LootObjectGUID)
		.PropertyHandle_Struct(PropertyHandle_MetaData)
		.ObjectType(ECategorySelector::Data)
	];
}
