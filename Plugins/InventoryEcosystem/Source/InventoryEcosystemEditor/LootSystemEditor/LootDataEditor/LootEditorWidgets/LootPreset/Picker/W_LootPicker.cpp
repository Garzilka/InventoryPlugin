#include "W_LootPicker.h"

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Category/LootSettingsListWidget.h"
#include "ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h"
#include "Widgets/Input/SComboButton.h"


void SMyComboMenu::EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID)
{
	OnClicked.ExecuteIfBound(ObjectGUID);
};

FText SW_LootPicker::OnGetText() const
{
	return Path;
}

void SW_LootPicker::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	
	LootAssetEditor = InArgs._LootAssetEditorRef;

	
	PropertyHandle_Row	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateDataRow, Row));
	if (!PropertyHandle_Row.IsValid()) return;
	
	PropertyHandle_Weight	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateDataRow, Weight));
	if (!PropertyHandle_Weight.IsValid()) return;
	
	SAssignNew(CM_Menu, SMyComboMenu)
		.LootAssetEditorRef(LootAssetEditor)
		.IsLoot(true)
		.OnClicked(this, &SW_LootPicker::EventSelectedButtonCallback);
	
	FName RowName;
	PropertyHandle_Row->GetValue(RowName);
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootData().Find(RowName))
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
								.Text(this, &SW_LootPicker::OnGetText)
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
									PropertyHandle_Row->CreatePropertyNameWidget()
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
										.OnGetMenuContent(this, &SW_LootPicker::OnGetKeyContent)
										.ContentPadding(FMargin(2.0f, 2.0f))
										.ButtonContent()
										[
											SNew(STextBlock)
											.Text(this, &SW_LootPicker::GetCurrentKeyDesc)
											.Font(IDetailLayoutBuilder::GetDetailFont())
										]
									]
								]
							]
							+ SVerticalBox::Slot()
							.Padding(0.f, 2.f, 0.f, 0.f)
							.AutoHeight()
							[					
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									PropertyHandle_Weight->CreatePropertyNameWidget()
								]
								+ SHorizontalBox::Slot()
								.Padding(5.f, 0.f, 0.f, 0.f)
								.AutoWidth()
								[
									PropertyHandle_Weight->CreatePropertyValueWidget()
								]
							]
						]
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SW_LootPicker::OnGetKeyContent() const
{
	return CM_Menu.ToSharedRef();
}

FText SW_LootPicker::GetCurrentKeyDesc() const
{
	return LootSpawner_Name;
}

void SW_LootPicker::EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID)
{
	PropertyHandle_Row->SetValue(ObjectGUID->GetLootObjectGUID());	
	
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootData().Find(ObjectGUID->GetLootObjectGUID()))
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

