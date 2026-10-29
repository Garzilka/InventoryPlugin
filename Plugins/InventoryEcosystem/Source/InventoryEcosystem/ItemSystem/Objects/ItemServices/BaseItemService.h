// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "InventoryEcosystem/InventoryCore/Object/CoreInventoryObject.h"
#include "BaseItemService.generated.h"

class UBaseItem;


UCLASS(Abstract, BlueprintType, Blueprintable)
class ITEMSYSTEM_API UBaseItemService : public UCoreInventoryObject
{
    GENERATED_BODY()
	
protected:
    UPROPERTY(Replicated)
    class UBaseItem* ItemOwner = nullptr;

public:
    UFUNCTION(BlueprintCallable)
    virtual void BeginPlay(UBaseItem* Item);

    UFUNCTION(BlueprintPure)
    virtual TArray<UObject*> GetAllChieldObject() { return {}; }

    UFUNCTION(BlueprintPure)
    virtual TArray<UBaseItem*> GetAttachedItem() { return {}; }
	
    UFUNCTION(BlueprintPure)
    UBaseItem* GetItemOwner();
	
    virtual AActor* GetOwner() override;
protected:
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
