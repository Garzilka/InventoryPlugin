#pragma once
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "SGraphPinNameList.h"
#include "K2Node_CallFunction.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"


class FCustomPinFuncTrade : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		if (!InPin) return nullptr;
		
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (InPin->PinType.PinCategory != K2Schema->PC_Name) return nullptr;
		
		const UEdGraphPin* DataPin = nullptr;
		FProperty* Prop = nullptr;
		
		if (InPin->GetOuter()->IsA(UK2Node_CallFunction::StaticClass()))
		{
			const UK2Node_CallFunction* CallFunctionNode = CastChecked<UK2Node_CallFunction>(InPin->GetOuter());
			if (!CallFunctionNode) return nullptr;
				
			const UFunction* FunctionToCall = CallFunctionNode->GetTargetFunction();
			if (!FunctionToCall) return nullptr;
				
			//Тут имя входящего параметра
			Prop = FunctionToCall->FindPropertyByName(InPin->PinName);
		}
			
		if (!Prop) return nullptr;

		const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
		auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
		if (!TradeSettings) return nullptr;
		
		TArray<TSharedPtr<FName>> RowNames;
		
		if (Prop->HasMetaData("CurrencyTypes"))
		{
			if (TradeSettings->CurrencySettings.Num() > 0)
			{
				for (auto& Pair : TradeSettings->CurrencySettings)
				{
					RowNames.Add(MakeShareable(new FName(Pair.Key.ToString())));
				}
				return SNew(SGraphPinNameList, InPin, RowNames);
			}
			RowNames.Add(MakeShareable(new FName("Empty")));
			return SNew(SGraphPinNameList, InPin, RowNames);
		}
		
		if (Prop->HasMetaData("TradeTypes"))
		{
			if (TradeSettings->TradeTypes.Num() > 0)
			{
				for (auto& TradeType : TradeSettings->TradeTypes)
				{
					RowNames.Add(MakeShareable(new FName(TradeType.ToString())));
				}
				return SNew(SGraphPinNameList, InPin, RowNames);
			}
			RowNames.Add(MakeShareable(new FName("Empty")));
			return SNew(SGraphPinNameList, InPin, RowNames);
		}
		
		return nullptr;
	}
};
