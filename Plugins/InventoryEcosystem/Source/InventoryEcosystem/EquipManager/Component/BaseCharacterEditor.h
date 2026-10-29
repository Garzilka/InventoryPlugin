// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMeshManager.h"
#include "BaseCharacterEditor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EQUIPMANAGER_API UBaseCharacterEditor : public UCharacterMeshManager
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_CharacterData, Category = "Настройки|MeshManager|Кастомизация")
	FName Gender = "Male";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки|MeshManager|Кастомизация")
	UDataTable* VisualData; 

public:	

	UFUNCTION(BlueprintPure)
	const FName& GetGender() const  {return Gender;}

	UFUNCTION(BlueprintPure)
	FMeshComponentData GetMeshByBodyType(FGameplayTag BodyTag);

protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void LoadDefault();

	UFUNCTION()
	void OnRep_CharacterData() {}
		
};
