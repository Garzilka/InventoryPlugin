// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTradeManager.h"

#include "BaseTradeComponent.h"
#include "InventoryEcosystem/LootSystem/WorldSubSystem/LootWorldSubSystem.h"
#include "Engine/World.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"

#pragma region ENGINE

UBaseTradeManager::UBaseTradeManager(){}




void UBaseTradeManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UBaseTradeManager, PlayerCurrency, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UBaseTradeManager, TradeState, COND_OwnerOnly);
}

void UBaseTradeManager::BeginPlay()
{
	Super::BeginPlay();
}
#pragma endregion ENGINE

#pragma region CURRENCY

FCurrencyCalculation UBaseTradeManager::GetTradeCurrencyDataBy(FName InTag) const
{
	for (auto LDataCopy : TradeGoldData.CurrencyData)
	{
		if (LDataCopy.CurrencyType != InTag) continue;

		return LDataCopy;
	};

	return FCurrencyCalculation();
}


UTradeSettings* UBaseTradeManager::GetTradeSettings()
{
	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	return LootSystemSetting->TradeSettings.LoadSynchronous();
}

int32 UBaseTradeManager::GetPlayerCurrency(FName CurrencyTag)
{
	if (!GetTradeSettings()) return 0;
	
	
	for (FCurrencyData LCurrencyData : PlayerCurrency.CurrencyData)
	{
		if (LCurrencyData.CyrrencyTag != CurrencyTag) continue;
		if (GetTradeSettings()->GetCurrencyTypeFor(CurrencyTag) == ECurrencyType::ECT_Value)
		{
			return LCurrencyData.GetValue();
		}
	}
	return 0;
}

TArray<FCurrencyData> UBaseTradeManager::GetTraderCurrency()
{
	return TradeState.TraderCurrencyData.CurrencyData;
}

int32 UBaseTradeManager::GetTraderCurrencyBy(FName CurrencyTag)
{
	for (auto LCurrencyData : TradeState.TraderCurrencyData.CurrencyData)
	{
		if (LCurrencyData.CyrrencyTag != CurrencyTag) continue;

		return LCurrencyData.GetValue();
	}
	return 0;
}

bool UBaseTradeManager::HasCurrencyByType(FName CurrencyTag, int32 TargetAmount)
{
	if (TargetAmount < 0) return false;
	int32 Value = GetPlayerCurrency(CurrencyTag);
	return (Value >= TargetAmount);
}

bool UBaseTradeManager::AddCurrencyByType(FName CurrencyTag, int32 TargetAmount)
{
	if (!GetTradeSettings()) return false;
#if WITH_EDITOR
	if (!ensure(HasAuthority())) { return false; };
#endif
	
#if WITH_EDITOR
	if (!ensure(TargetAmount > 0)) { return false; };
#endif
	
	if (GetTradeSettings()->GetCurrencyTypeFor(CurrencyTag) == ECurrencyType::ECT_Value)
	{
		FCurrencyData& LCurrencyData = PlayerCurrency.FindOrAdd(CurrencyTag);
		LCurrencyData.SetValue((LCurrencyData.GetValue() + TargetAmount));
		// OnRep_Currency();
		return true;
	}
	return false;
}

bool UBaseTradeManager::SetCurrencyByType(FName CurrencyTag, int32 TargetAmount)
{
#if WITH_EDITOR
	if (!ensure(HasAuthority())) { return false; };
#endif
	
#if WITH_EDITOR
	if (!ensure(TargetAmount <= 0)) { return false; };
#endif
	
	if (GetTradeSettings()->GetCurrencyTypeFor(CurrencyTag) == ECurrencyType::ECT_Value)
	{
		FCurrencyData& LCurrencyData = PlayerCurrency.FindOrAdd(CurrencyTag);
		if (LCurrencyData.GetValue() < TargetAmount) return false;

		LCurrencyData.SetValue(TargetAmount);
		// OnRep_Currency();
		return true;
	}
	return false;
}

bool UBaseTradeManager::SubstructCurrencyByType(FName CurrencyTag, int32 TargetAmount)
{
#if WITH_EDITOR
	if (!ensure(HasAuthority())) { return false; };
#endif
	

	if (GetTradeSettings()->GetCurrencyTypeFor(CurrencyTag) == ECurrencyType::ECT_Value)
	{
		FCurrencyData& LCurrencyData = PlayerCurrency.FindOrAdd(CurrencyTag);
		if (LCurrencyData.GetValue() < TargetAmount) return false;

		LCurrencyData.SetValue(LCurrencyData.GetValue() - TargetAmount);
		// OnRep_Currency();
		return true;
	}
	
	return false;
}

#pragma endregion CURRENCY

void UBaseTradeManager::OnRep_TradeState()
{
	TradeOpen.Broadcast(this);
}


void UBaseTradeManager::OpenTrade_TradeComponent(UBaseTradeComponent* TradeComponent)
{
	if (!IsValid(TradeComponent)) return;
	
	if (!HasAuthority())
	{
		Server_OpenTrade_TradeComponent(TradeComponent);
		return;
	}
	
	TradeState.AnotherTradeComponent = TradeComponent;
	TradeState.TraderCurrencyData = TradeState.AnotherTradeComponent->GetTradeCurrency(Cast<ACharacter>(GetOwner()));
	
	MakeAnotherInventory(TradeComponent->GetOwner());
	
	for (auto GenerateItem : TradeState.AnotherTradeComponent->GenereteItems(GetOwningPlayerController()))
	{
		TradeState.AnotherInventory->AddTryItem(GenerateItem, true, false);
	}
	
	TradeState.CurrentTrade = ETradeType::ETradeType_P_TO_NPC;
	TradeComponent->GetOwner()->ForceNetUpdate();
	
	OnRep_TradeState();
}

void UBaseTradeManager::OpenTrade_Inventory(UBaseLineInventory* InventoryComponent)
{
	if (!IsValid(InventoryComponent)) return;
	
	if (!HasAuthority())
	{
		Server_OpenTrade_Inventory(InventoryComponent);
		return;
	}
	
	TradeState.AnotherInventory = InventoryComponent;

	TradeState.CurrentTrade = ETradeType::ETradeType_P_TO_NPC;

	OnRep_TradeState();
}

void UBaseTradeManager::Server_OpenTrade_TradeComponent_Implementation(UBaseTradeComponent* TradeComponent)
{
	OpenTrade_TradeComponent(TradeComponent);
}

void UBaseTradeManager::Server_OpenTrade_Inventory_Implementation(UBaseLineInventory* TradeComponent)
{
	OpenTrade_Inventory(TradeComponent);
}

void UBaseTradeManager::MakeAnotherInventory(AActor* For)
{
	UBaseLineInventory* LTradeInventory = NewObject<UBaseLineInventory>(For, TraderInventoryClass);
		
	TArray<UBaseItem*> Old;
	LTradeInventory->RegisterComponent();
	LTradeInventory->SetSize(InventorySize, Old);
	TradeState.AnotherInventory = LTradeInventory;
}
