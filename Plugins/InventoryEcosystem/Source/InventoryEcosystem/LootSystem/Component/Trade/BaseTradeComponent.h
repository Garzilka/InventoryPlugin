// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Components/ActorComponent.h"
#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootComponent.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystem/LootSystem/Object/LootObjectGUID/LootObjectGUID.h"
#include "BaseTradeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOOTSYSTEM_API UBaseTradeComponent : public UBaseLootComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки",meta = (DisplayPriority = -1, DisplayName = "Тип торговли"))
	FCurrencyTraderData CurrencyData;

public:	
	UBaseTradeComponent();

	UFUNCTION(BlueprintPure)
	virtual FCurrencysRepData GetTradeCurrency(APawn* Instigator);

	UFUNCTION(BlueprintPure)
	virtual int32 GetScore(APawn* Instigator) {return 0;}


protected:
	virtual void BeginPlay() override;
		
};
