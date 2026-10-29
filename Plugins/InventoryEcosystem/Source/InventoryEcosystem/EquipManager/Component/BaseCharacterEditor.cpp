// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterEditor.h"

#include "InventoryEcosystem/EquipManager/Library/EquipManagerTool.h"


#pragma region ENGINE

void UBaseCharacterEditor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	LoadDefault();
}

#pragma endregion ENGINE

#pragma region TOOLS

void UBaseCharacterEditor::LoadDefault()
{
	for (FGameplayTag Tag : UEquipManagerTool::GetBodyTags())
	{
		AddMesh(GetMeshByBodyType(Tag));
	}
}

FMeshComponentData UBaseCharacterEditor::GetMeshByBodyType(FGameplayTag BodyTag)
{
	if (VisualData == nullptr)
	{
		return FMeshComponentData();
	}
	FCharacterEditorGenderTable* GenderData = VisualData->FindRow<FCharacterEditorGenderTable>(Gender, FString());

	if (!(GenderData && GenderData->GenderData))
	{
		return FMeshComponentData();
	}
	FMeshSkinDataAsset* BodyData = GenderData->GenderData->BodyData.Find(BodyTag);
	
	FMeshComponentData Result;
	if (BodyData)
	{
		Result.MeshRef = BodyData->Mesh;
		Result.BodyTag = BodyTag;
	}
	return Result;
}
#pragma endregion TOOLS


#pragma region REPLICATED

void UBaseCharacterEditor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseCharacterEditor, Gender);
}

bool UBaseCharacterEditor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	return WroteSomething;
}
#pragma endregion REPLICATED