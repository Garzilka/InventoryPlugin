// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryEcosystem/InventoryCore/Object/CoreInventoryObject.h"

#include "InventoryEcosystem/ItemSystem/Data/InventoryData.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"

#include "BaseItem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemUpdateSignature, class UBaseItem*, Item);

UCLASS(Abstract, BlueprintType, Blueprintable, meta=(DisplayThumbnail = "true"))
class ITEMSYSTEM_API UBaseItem : public UCoreInventoryObject
{
    GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
    FItemUpdateSignature OnItemUpdate;
    
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
    FItemUpdateSignature OnCaptureUpdate;
	
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
    FItemUpdateSignature OnItemEquiped;
	
    UPROPERTY(BlueprintAssignable, Category = "Delegates")
    FItemUpdateSignature OnItemUnequiped;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки", meta = (DisplayName = "Основные"))
    FDefaultItemData DefaultSettings;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки")
    bool bIsUseRender = true;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки", meta = (EditCondition="bIsUseRender", EditConditionHides))
    float RenderOrtoWidth = 80.f;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки", meta = (EditCondition="bIsUseRender", EditConditionHides))
    FTransform RenderOffset;
	
    UPROPERTY()
    bool IsValidRender = false;
	
    UPROPERTY()
    UTextureRenderTarget2D* Render = nullptr;

    UPROPERTY(ReplicatedUsing = OnRep_Amount, SaveGame)
    int32 Amount = 1;

    UPROPERTY(Replicated, SaveGame)
    bool bIsRotate;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Replicated, Category = "Services|Base")
    class UBasePickupService* PickupService;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Replicated, Category = "Services|Base")
    class UBaseWorldItemService* WorldPlacementService;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Replicated, Category = "Services|Base")
    class UBaseQualityService* QualityService;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Replicated, Category = "Services|Addition")
    TArray<UBaseItemService*> Services;

public:
    virtual TArray<UObject*> GetAttachedSubObject() override;
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
	
    UFUNCTION(BlueprintPure)
    bool IsUseRender() const { return bIsUseRender; };
	
    UFUNCTION(BlueprintPure)
    float GetRenderOrthoWidth() const { return RenderOrtoWidth; };

    void SetRender(UTextureRenderTarget2D* InRender);
    
    UFUNCTION(BlueprintPure)
    bool GetRender(UTextureRenderTarget2D*& InRender)
    {
        if (!IsValidRender)
        {
            InRender = nullptr;
            return false;
        }
        InRender = Render;
        return true;
    }

    UTextureRenderTarget2D* GetRenderTargetRef()
    {
        return Render;
    }
	
    UFUNCTION(BlueprintPure)
    FTransform GetRenderOffset() const { return RenderOffset; };
	
    UFUNCTION(BlueprintPure)
    virtual TArray<UBaseItem*> GetAttachedItem(bool IncludedSelf = false);

    UFUNCTION(BlueprintCallable)
    void BeginPlay();

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    FText GetItemName() { return DefaultSettings.ItemName; }

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    float GetItemWeight(bool IsFull = true);

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    FText GetDescription() { return DefaultSettings.Description; }

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Description")
    void SetIcon(UTexture* Texture);

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    UTexture* GetIcon() { return DefaultSettings.Icon; }

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    FGameplayTag GetRarity() { return DefaultSettings.RarityType; }

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Description")
    FIntPoint GetDimensions() { return (bIsRotate ? FIntPoint(DefaultSettings.Dimensions.Y, DefaultSettings.Dimensions.X) : DefaultSettings.Dimensions); }

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Amount")
    int32 SetAmount(int32 NewAmmout);

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Amount")
    bool StackItems(UBaseItem* TargetItem);

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Amount")
    bool CanBeStack(UBaseItem* TargetItem);

    /*
    * ���������� �������
    */
    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Amount")
    int32 SubstructAmount(int32 InAmount);

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Amount")
    int32 GetAmount() const { return Amount; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Amount")
    bool IsStackable() { return DefaultSettings.bIsStack; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Amount")
    int32 GetStackSize() { return DefaultSettings.StackSize; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Tools")
    FGameplayTag GetItemType() { return DefaultSettings.ItemType; }

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Services")
    TArray<UBaseItemService*> GetServices() { return Services; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Services")
    UBaseQualityService* GetQualityService() { return QualityService; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Services")
    UBasePickupService* GetPickupService() { return PickupService; };

    UFUNCTION(BlueprintPure, Category = "UnrealInventory|Inventory|Item|Services")
    UBaseWorldItemService* GetWorldPlacementService() { return WorldPlacementService; };

    UFUNCTION(BlueprintCallable, Category = "UnrealInventory|Inventory|Item|Services", meta = (DeterminesOutputType = "ClassService", DynamicOutputParam = "Service"))
    void GetServiceTyped(TSubclassOf<UBaseItemService> ClassService, UBaseItemService*& Service, bool CheckChild);

    template <class T>
    FORCEINLINE T* GetServiceTyped(TSubclassOf<UBaseItemService> Sevice = NULL)
    {
        for (auto Service : Services)
        {
            if (Sevice != NULL)
            {
                if (Service && Service->IsA(Sevice))
                {
                    return Cast<T>(Service);
                };
            }
            else
            {
                T* Result = Cast<T>(Service);
                if (Result != nullptr) return Result;
            }
        }

        return nullptr;
    }
	
    virtual void Serialize(FArchive& Ar) override;
protected:
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Amount() { OnItemUpdate.Broadcast(this); };
};
