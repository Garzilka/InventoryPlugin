#include "W_PresetPicker.h"

#include "W_LootPicker.h"

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Category/LootSettingsListWidget.h"
#include "ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h"
#include "Widgets/Input/SComboButton.h"




FText SW_PresetPicker::OnGetText() const
{
	return Path;
}

void SW_PresetPicker::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	
	LootAssetEditor = InArgs._LootAssetEditorRef;
	
	PropertyHandle_PresetGUID	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, PresetGUID));
	if (!PropertyHandle_PresetGUID.IsValid()) return;
	
	SAssignNew(CM_Menu, SMyComboMenu)
		.LootAssetEditorRef(LootAssetEditor)
		.IsLoot(false)
		.OnClicked(this, &SW_PresetPicker::EventSelectedButtonCallback);
	
	FName RowName;
	PropertyHandle_PresetGUID->GetValue(RowName);
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootPresets().Find(RowName))
	{
		LootSpawner_Name = TextData->MetaData.Object_Name;
		Path = FText::FromString(TextData->MetaData.Category.ToString() + "|"+ LootSpawner_Name.ToString());
	}
	else
	{
		LootSpawner_Name = FText::FromString("None");
		Path = FText::FromString("None");
	}
	
	ChildSlot
	[
		SNew(SBox)	
		.Padding(2.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBorder)		
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.3f))
					.Padding(5.f)
					[
						SNew(SBox)					
						.Padding(5.f)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(STextBlock)
								.Text(this, &SW_PresetPicker::OnGetText)
								.ColorAndOpacity(FLinearColor(0.15f, 0.15f, 0.15f))
								
							]
							+ SVerticalBox::Slot()
							.Padding(0.f, 2.f, 0.f, 0.f)
							.AutoHeight()
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									PropertyHandle_PresetGUID->CreatePropertyNameWidget()
								]
								+ SHorizontalBox::Slot()
								.Padding(5.f, 0.f, 0.f, 0.f)
								.AutoWidth()
								[
									SNew(SBox)
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Left)
									[
										SAssignNew(CB_Selector, SComboButton)
										.OnGetMenuContent(this, &SW_PresetPicker::OnGetKeyContent)
										.ContentPadding(FMargin(2.0f, 2.0f))
										.ButtonContent()
										[
											SNew(STextBlock)
											.Text(this, &SW_PresetPicker::GetCurrentKeyDesc)
											.Font(IDetailLayoutBuilder::GetDetailFont())
										]
									]
								]
							]
						]
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SW_PresetPicker::OnGetKeyContent() const
{
	return CM_Menu.ToSharedRef();
}

FText SW_PresetPicker::GetCurrentKeyDesc() const
{
	return LootSpawner_Name;
}

void SW_PresetPicker::EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID)
{
	PropertyHandle_PresetGUID->SetValue(ObjectGUID->GetLootObjectGUID());	
	
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootPresets().Find(ObjectGUID->GetLootObjectGUID()))
	{
		LootSpawner_Name = TextData->MetaData.Object_Name;
		Path = FText::FromString(TextData->MetaData.Category.ToString() + "|"+ LootSpawner_Name.ToString());
	}
	else
	{
		LootSpawner_Name = FText::FromString("None");
		Path = FText::FromString("None");
	}
	
	CB_Selector->SetIsOpen(false);
};

