// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryCore/Component/SceneInventoryCoreComponent.h"
#include "InventoryEcosystem/InteractSystem/Data/InteractData.h"
#include "BaseInteractComponent.generated.h"




UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent), HideCategories = ("Component", "Sockets", "Collision", "AssetUserData", "Component Tick", "ComponentTick", "Component Replication", "ComponentReplication", "Cooking", "Activation", "Variable"))
class INTERACTSYSTEM_API UBaseInteractComponent : public USceneInventoryCoreComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FInteractFocusSignature OnBeginFocus;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FInteractFocusSignature OnFocusLost;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FInteractedSignature OnInteracted;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FInteractedChangeStateSignature OnBeginInteract;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FInteractedChangeStateSignature OnEndInteract;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Настройки|Компонент взаимодействия", ReplicatedUsing = OnRep_UpdateInteractDescription)
	FInteractDescription Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Настройки|Компонент взаимодействия", ReplicatedUsing = OnRep_UpdateInteractData, meta = (DisplayName = "Описание взаимодействия"))
	FInteractSettings InteractSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Настройки|Компонент взаимодействия", ReplicatedUsing = OnRep_UpdateAngleSettings, meta = (DisplayName = "Настройки ограничения угла"))
	FInteractAngleSettings AngleSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Настройки|Компонент взаимодействия", meta = (DisplayName = "Укажите компонент анимаций"))
	UInteractAnimationComponent* InteractAnimationComponent;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UpdateInteractors)
	TArray<APawn*> CurrentInteractors;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockInteractFor)
	TArray<FBlockInteractInfo> BlockInteractFor;

	UPROPERTY()
	float CurrentInteractTime = 0.f;

	UPROPERTY()
	bool InInteract = false;

public:
	UBaseInteractComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool SetInteractNameText(FText NewNameText);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool SetInteractActionText(FText NewActionText);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void SetInteractPresset(FName InteractPresset);

	UFUNCTION(BlueprintPure)
	FInteractSettings GetInteractSettings() const {return InteractSettings;};
	
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	FText GetInteractableNameText() { return Description.NameText; }

	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	FText GetInteractableActionText() { return Description.ActionText; }

	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	FInteractAngleSettings GetAngleSettings() { return AngleSettings; }

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void BeginFocus(APawn* CharacterInstigator);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void EndFocus(APawn* CharacterInstigator);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void Interact(APawn* CharacterInstigator, EInteractType InteractType);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void StartInteract(APawn* CharacterInstigator);

	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void EndInteract(APawn* CharacterInstigator);
	
	/**	*/
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool HasInteractFlag(EInteractType Type) const;

	/**	*/
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool CanInteractFor(APawn* CharacterInstigator) const;

	/**	*/
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool CanViewFor(APawn* CharacterInstigator) const;

	/**/
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool CanBePressed() const;

	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	bool IsOutLine(UPrimitiveComponent* Primitive);
	
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	float GetTimeToInteract() const;
	
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	float GetInteractPercent() const;
	
	UFUNCTION(BlueprintPure, Category = "InventoryEcosystem|Interact|InteractComponent")
	UInteractAnimationComponent* GetInteractAnimationComponent() const { return InteractAnimationComponent;};
	
	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void ActivateOutLine();
	
	UFUNCTION(BlueprintCallable, Category = "InventoryEcosystem|Interact|InteractComponent")
	void DeactivateOutLine();

protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION()
	void OnRep_UpdateInteractDescription() {};

	UFUNCTION()
	void OnRep_UpdateInteractData() {};

	UFUNCTION()
	void OnRep_UpdateAngleSettings() {};

	UFUNCTION()
	void OnRep_UpdateInteractors() {};

	UFUNCTION()
	void OnRep_UpdateCurrentViewer() {};

	UFUNCTION()
	void OnRep_BlockInteractFor() {};
	
};
