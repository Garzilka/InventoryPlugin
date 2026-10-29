// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseSpatialInventory.h"
#include "InventoryEcosystem/LootSystem/Data/TradeData.h"

#include "Components/ActorComponent.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "BaseTradeManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTradeOpenSignature, class UBaseTradeManager*, TradeManager);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOOTSYSTEM_API UBaseTradeManager : public UInventoryCoreComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable)
	FTradeOpenSignature TradeOpen;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseLineInventory> TraderInventoryClass = UBaseSpatialInventory::StaticClass();
	
	UPROPERTY(EditDefaultsOnly)
	FIntPoint InventorySize = FIntPoint(8, 20);

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CurrencyItemType;
	
	UPROPERTY(BlueprintReadOnly)
	FCurrencysRepData PlayerCurrency;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TradeState)
	FTradeState TradeState;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TradeGoldData)
	FCurrenciesCalculation TradeGoldData;

	
public:	
	UBaseTradeManager();

	// void AddCurrencyFromItem(UBaseItem* Item);
	
	UFUNCTION(BlueprintPure)
	FCurrencyCalculation GetTradeCurrencyDataBy(UPARAM(Meta = (CurrencyTypes)) FName InTag) const;
	
	UFUNCTION(BlueprintCallable)
	virtual int32 GetPlayerCurrency(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag);
	
	UFUNCTION(BlueprintCallable)
	virtual TArray<FCurrencyData> GetTraderCurrency();
	
	UFUNCTION(BlueprintCallable)
	virtual int32 GetTraderCurrencyBy(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag);
	
	UFUNCTION(BlueprintCallable)
	virtual bool HasCurrencyByType(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag, int32 TargetAmount);
	
	UFUNCTION(BlueprintCallable)
	virtual bool AddCurrencyByType(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag, int32 TargetAmount);
	
	UFUNCTION(BlueprintCallable)
	virtual bool SetCurrencyByType(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag, int32 TargetAmount);
	
	UFUNCTION(BlueprintCallable)
	virtual bool SubstructCurrencyByType(UPARAM(Meta = (CurrencyTypes)) FName CurrencyTag, int32 TargetAmount);
	
	UFUNCTION(BlueprintCallable)
	virtual void OpenTrade_TradeComponent(UBaseTradeComponent* TradeComponent);
	
	UFUNCTION(BlueprintCallable)
	UTradeSettings* GetTradeSettings();
	
	UFUNCTION(BlueprintCallable)
	virtual void OpenTrade_Inventory(UBaseLineInventory* InventoryComponent);

	UFUNCTION()
	void OnRep_TradeGoldData() {};
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_TradeState();
	
	UFUNCTION(Server, Reliable)
	void Server_OpenTrade_TradeComponent(UBaseTradeComponent* TradeComponent);
	void Server_OpenTrade_TradeComponent_Implementation(UBaseTradeComponent* TradeComponent);

	UFUNCTION(Server, Reliable)
	void Server_OpenTrade_Inventory(UBaseLineInventory* TradeComponent);
	void Server_OpenTrade_Inventory_Implementation(UBaseLineInventory* TradeComponent);

	void MakeAnotherInventory(AActor* For);
};
