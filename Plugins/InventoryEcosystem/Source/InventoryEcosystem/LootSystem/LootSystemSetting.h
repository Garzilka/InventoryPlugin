#pragma once
#include "CoreMinimal.h"
#include "Data/CurrencyData.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "Data/LootSystemData.h"
#include "LootSystemSetting.generated.h"


UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Inventory System"))
class LOOTSYSTEM_API ULootSystemSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(Config, EditAnywhere, Category = "Loot System")
	TSoftObjectPtr<ULootData> LootData;

	UPROPERTY(Config, EditAnywhere, Category = "Loot System")
	TSoftObjectPtr<UTradeSettings> TradeSettings;


	static const ULootSystemSetting* Get();
};