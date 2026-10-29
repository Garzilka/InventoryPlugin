#include "W_LootMetaData.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_StandardizedRow.h"


void SW_LootMetaData::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditor;	
	ObjectType = InArgs._ObjectType;
	LootObjectGUID = InArgs._LootObjectGUID;
	
	PropertyHandle_Category	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FObjectMetaData, Category));
	if (!PropertyHandle_Category.IsValid()) return;
	
	FSimpleDelegate OnCategoryChange = FSimpleDelegate::CreateSP(this, &SW_LootMetaData::CategoryChanged);
	PropertyHandle_Category->SetOnPropertyValueChanged(OnCategoryChange);
	
	PropertyHandle_Name	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FObjectMetaData, Object_Name));
	if (!PropertyHandle_Name.IsValid()) return;
	
	FSimpleDelegate OnNameChange = FSimpleDelegate::CreateSP(this, &SW_LootMetaData::NameChanged);
	PropertyHandle_Name->SetOnPropertyValueChanged(OnNameChange);

	PropertyHandle_Name->GetValue(LootDataObject_CurrentName);
	PropertyHandle_Category->GetValue(LootDataObject_CurrentCategory);

	LootDataObject_OldName = LootDataObject_CurrentName;
	LootDataObject_OldCategory = LootDataObject_CurrentCategory;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SW_StandardizedRow)
			.DisplayName(PropertyHandle_Category->GetPropertyDisplayName())
			.PropertyHandle(PropertyHandle_Category)
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 10.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SW_StandardizedRow)
			.DisplayName(PropertyHandle_Name->GetPropertyDisplayName())
			.PropertyHandle(PropertyHandle_Name)
		]
	];
}

void SW_LootMetaData::CategoryChanged()
{
	LootDataObject_OldCategory = LootDataObject_CurrentCategory;
	PropertyHandle_Category->GetValue(LootDataObject_CurrentCategory);
	
	if (LootDataObject_OldCategory == LootDataObject_CurrentCategory) return;

	if (!LootObjectGUID.IsValid()) return;
	LootAssetEditor.Pin()->Event_ObjectChangeCategory(ObjectType, LootObjectGUID, LootDataObject_OldCategory, LootDataObject_CurrentCategory);
}

void SW_LootMetaData::NameChanged()
{
	LootDataObject_OldName = LootDataObject_CurrentName;
	PropertyHandle_Name->GetValue(LootDataObject_CurrentName);
	
	if (LootDataObject_OldName.ToString() == LootDataObject_CurrentName.ToString()) return;

	if (!LootObjectGUID.IsValid()) return;
	
	LootAssetEditor.Pin()->Event_ObjectChangeCategory(ObjectType, LootObjectGUID, LootDataObject_OldCategory, LootDataObject_CurrentCategory);
}