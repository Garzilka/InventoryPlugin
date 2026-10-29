#include "W_RespawnData.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"



void SW_RespawnData::Construct(const FArguments& InArgs)
{
    PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
    LootAssetEditor = InArgs._LootAssetEditorRef;

    PropertyHandle_TimeToRestart = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRespawnLootData, TimeToRestart));
    if (!PropertyHandle_TimeToRestart.IsValid()) return;

    PropertyHandle_RespawnLootType = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRespawnLootData, RespawnLootType));
    if (!PropertyHandle_RespawnLootType.IsValid()) return;
    
    ChildSlot
    [
        SNew(SBox)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                PropertyHandle_RespawnLootType->CreatePropertyValueWidget()
            ]
            + SHorizontalBox::Slot()
            .Padding(5.f, 0.f, 0.f, 0.f)
            .AutoWidth()
            [
                SNew(SBox)
                .Visibility(this, &SW_RespawnData::GetVisibilityTimeRespawn)
                [
                    PropertyHandle_TimeToRestart->CreatePropertyValueWidget()
                ]
            ]
        ]
    ];
}

EVisibility SW_RespawnData::GetVisibilityTimeRespawn() const
{
    bool Result = false;
	
    uint8 Res = 0;
    if (PropertyHandle_RespawnLootType.IsValid())
    {
        PropertyHandle_RespawnLootType->GetValue(Res);
    }
    EGenerationLootType GenerateType = (EGenerationLootType)Res;
	
    return ((GenerateType == EGenerationLootType::EGLT_Timer || GenerateType == EGenerationLootType::EGLT_TimerAfterPickup) ? EVisibility::Visible : EVisibility::Collapsed);
}