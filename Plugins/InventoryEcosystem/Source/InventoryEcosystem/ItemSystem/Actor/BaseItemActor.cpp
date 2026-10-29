// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItemActor.h"

#include "InventoryEcosystem/InteractSystem/Components/BaseInteractComponent.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BasePickupService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"
#include "Components/BillboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StreamableManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstance.h"
#include "UObject/ConstructorHelpers.h"

#pragma region ENGINE

ABaseItemActor::ABaseItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	UBillboardComponent* LBilbord = CreateDefaultSubobject<UBillboardComponent>(TEXT("Bilbord"));
	LBilbord->SetupAttachment(GetRootComponent());
	LBilbord->SetRelativeLocation(FVector(0, 0, 25.f));
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> LoadTexture(TEXT("/InventoryEcosystem/zOther/AssetIcons/ItemBilbord.ItemBilbord"));
	if (!ensure(LoadTexture.Object != nullptr)) return;
	LBilbord->SetSprite(LoadTexture.Object);
}


void ABaseItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	if (InteractComponent)
	{
		InteractComponent->OnInteracted.AddUniqueDynamic(this, &ABaseItemActor::Event_Interacted);
	}
	
	if (!DoNeedRespawnOnBeginPlay) return;

	//If not spawned, restart item
	if (ItemRef && ItemRef->GetOwner() && ItemRef->GetOwner()->GetOwner() == nullptr)
	{
		ItemRef = nullptr;
	}

	SpawnItem();
	LoadMesh(IsNetMode(NM_DedicatedServer));
}

void ABaseItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ABaseItemActor::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}


void ABaseItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void ABaseItemActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!ItemContext.IsValid())
	{
		return;
	}
	
	if (ItemRef && ItemContext.GetItemClass() != ItemRef->GetClass())
	{
		ItemRef = nullptr;
	}

	if (UKismetSystemLibrary::IsValidClass(ItemContext.GetItemClass()))
	{
		UBaseItem* ItemClass = Cast<UBaseItem>(ItemContext.GetItemClass().GetDefaultObject());
		if (ItemClass && ItemClass->GetWorldPlacementService())
		{
			if (ItemClass->GetWorldPlacementService()->GetClassSpawn() != GetClass())
			{
				auto Actor = GetWorld()->SpawnActorDeferred<ABaseItemActor>(ItemClass->GetWorldPlacementService()->GetClassSpawn(), GetActorTransform());
				Actor->ItemContext = ItemContext;
				Actor->FinishSpawning(GetActorTransform());
				Actor->SpawnItem();
				Actor->LoadMesh(true);
				GEditor->SelectActor(Actor, true, true);
				Destroy();
				return;
			}
		}
	}
	SpawnItem();
	LoadMesh(true);
}
#endif
#pragma endregion ENGINE

#pragma region TOOLS

UBaseItem* ABaseItemActor::GetItem() const
{
	return ItemRef;
}

void ABaseItemActor::SpawnItem()
{
	if (IsValid(ItemRef)) return;

	ItemRef = UInventoryLibrary::MakeItem(this, ItemContext);
	SetupItem(ItemRef);
}


void ABaseItemActor::OnRep_Item()
{
	LoadMesh();
}


void ABaseItemActor::LoadMesh(bool DoForce)
{
	if (!(ItemRef && ItemRef->GetWorldPlacementService() && ItemRef->GetWorldPlacementService()->IsMakeMeshComponent())) return;
	
	TSoftObjectPtr<UStreamableRenderAsset> MeshPath = TObjectPtr<UStreamableRenderAsset>();
	auto PlaceType = ItemRef->GetWorldPlacementService()->GetPlaceType();
	
	if (PlaceType == EItemPlacementType::EIPT_StaticMesh)
	{
		MeshPath = ItemRef->GetWorldPlacementService()->GetStaticMesh();
	}
	
	if (PlaceType == EItemPlacementType::EIPT_SkeletalMesh)
	{
		MeshPath = ItemRef->GetWorldPlacementService()->GetSkeletalMesh();
	}
	
	if (MeshPath.IsNull()) return;
	
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();

	if (DoForce && !MeshPath.Get())
	{
		MeshPath.LoadSynchronous();
	}
	
	if (MeshPath.Get())
	{
		if (PlaceType == EItemPlacementType::EIPT_StaticMesh)
		{
			Async_LoadStaticMesh(MeshPath.ToSoftObjectPath());
		}
	
		if (PlaceType == EItemPlacementType::EIPT_SkeletalMesh)
		{
			Async_LoadSkeletalMesh(MeshPath.ToSoftObjectPath());
		}
		return;
	}
	
	if (PlaceType == EItemPlacementType::EIPT_StaticMesh)
	{
		AssetLoader.RequestAsyncLoad(MeshPath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ABaseItemActor::Async_LoadStaticMesh, MeshPath.ToSoftObjectPath()));
	}
	else
	{
		AssetLoader.RequestAsyncLoad(MeshPath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject	(this,	&ABaseItemActor::Async_LoadSkeletalMesh,	MeshPath.ToSoftObjectPath()));
	}
}

void ABaseItemActor::Async_LoadStaticMesh(FSoftObjectPath Reference)
{
	if (!(ItemRef && ItemRef->GetWorldPlacementService() && ItemRef->GetWorldPlacementService()->IsMakeMeshComponent())) return;
	UStaticMesh* CachedMesh = Cast<UStaticMesh>(StaticLoadObject(UObject::StaticClass(), nullptr, *(Reference.ToString())));
	if (!CachedMesh) return;

	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(MeshComponent))
	{
		Mesh->SetStaticMesh(CachedMesh);
		return;
	}
	
	if (MeshComponent)
	{
		MeshComponent->SetVisibility(false);
		MeshComponent->DestroyComponent();
		MeshComponent->UnregisterComponent();
		MeshComponent->MarkAsGarbage();
		MeshComponent = nullptr;
	}
	
	MeshComponent = NewObject<UStaticMeshComponent>(this);
	MeshComponent->RegisterComponentWithWorld(GetWorld());
	MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(MeshComponent))
	{
		Mesh->SetStaticMesh(CachedMesh);
		Mesh->SetCollisionProfileName(UInventoryLibrary::GetDefaultInteractProfile());
		Mesh->ComponentTags.Add("OutLine");
		for (auto Material : ItemRef->GetWorldPlacementService()->GetOverrideMaterial())
		{
			Mesh->SetMaterialByName(Material.Key, Material.Value.LoadSynchronous());
		}
	}

}


void ABaseItemActor::Async_LoadSkeletalMesh(FSoftObjectPath Reference)
{
	USkeletalMesh* CachedMesh = Cast<USkeletalMesh>(StaticLoadObject(UObject::StaticClass(), nullptr, *(Reference.ToString())));
	if (!CachedMesh) return;

	if (USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(MeshComponent))
	{
		Mesh->SetSkeletalMesh(CachedMesh);
		return;
	}
	if (MeshComponent)
	{
		MeshComponent->SetVisibility(false);
		MeshComponent->DestroyComponent();
		MeshComponent->UnregisterComponent();
		MeshComponent->MarkAsGarbage();
		MeshComponent = nullptr;
	}
	
	MeshComponent = NewObject<USkeletalMeshComponent>(this);
	MeshComponent->RegisterComponentWithWorld(GetWorld());
	MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

	if (USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(MeshComponent))
	{
		Mesh->SetSkeletalMesh(CachedMesh);
		Mesh->SetCollisionProfileName(UInventoryLibrary::GetDefaultInteractProfile());
		Mesh->ComponentTags.Add("OutLine");
		for (auto Material : ItemRef->GetWorldPlacementService()->GetOverrideMaterial())
		{
			Mesh->SetMaterialByName(Material.Key, Material.Value.LoadSynchronous());
		}
	}
}

bool ABaseItemActor::SetupItem(UBaseItem* NewItem, bool NeedSwitchParren)
{
	if (!(HasAuthority() && IsValid(NewItem))) return false;
	if (!IsValid(NewItem->GetWorldPlacementService())) return false;

	NewItem->GetWorldPlacementService()->SetSpawnedRef(this);

	DoNeedRespawnOnBeginPlay = false;
	ItemRef = NewItem;

	if(NeedSwitchParren)
		ItemRef->SwitchParrent(this);

	ForceNetUpdate();
	return true;
}

bool ABaseItemActor::I_RemoveItem_Implementation(UCoreInventoryObject* InObject)
{
	if (InObject != GetItem()) return false;

	Destroy(); 
	return true;
}

void ABaseItemActor::Event_Interacted(APawn* InCharacterInstigator, UBaseInteractComponent* InInteractComponent, EInteractType InInteractType)
{
	if (!(GetItem() && GetItem()->GetPickupService())) return;
	
	GetItem()->GetPickupService()->PickupItem(InCharacterInstigator);
}

#pragma endregion TOOLS
