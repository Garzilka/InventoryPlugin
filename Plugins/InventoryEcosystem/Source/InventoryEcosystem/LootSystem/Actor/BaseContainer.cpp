// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseContainer.h"

#include "TimerManager.h"
#include "ZoneLootManager.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootManager.h"
#include "InventoryEcosystem/LootSystem/Data/LootSaveData.h"
#include "InventoryEcosystem/LootSystem/WorldSubSystem/LootWorldSubSystem.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

#pragma region ENGINE

ABaseContainer::ABaseContainer()
{
    bReplicates = true;
    SetNetUpdateFrequency(2.f);
    RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
    LootComponent = CreateDefaultSubobject<UBaseLootComponent>("LootComponent");
}

void ABaseContainer::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority() && LootComponent->GetRespawnLootType() != EGenerationLootType::EGLT_BeginPlay)
    {
    	MakeSphereTrigger();
    }
    
    if (LootComponent->GetRespawnLootType() == EGenerationLootType::EGLT_BeginPlay)
    {
    	if (LootComponent->IsUniqueLoot())
    	{
            FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &ABaseContainer::RegisterLoginPlayer);
            FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &ABaseContainer::RegisterLogoutPlayer);
    	}
    	else
    	{
    	    GenereteItems(nullptr);
    	}
    }
	
}

#pragma endregion ENGINE

#pragma region LOGIN_LOGOUT


void ABaseContainer::RegisterLoginPlayer(AGameModeBase* GameMode, APlayerController* OutPlayer)
{
}

void ABaseContainer::RegisterLogoutPlayer(AGameModeBase* GameMode, AController* OutPlayer)
{
}

#pragma endregion LOGIN_LOGOUT

#pragma region REPLICATE

void ABaseContainer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
    DOREPLIFETIME(ABaseContainer, Inventory);
    DOREPLIFETIME_CONDITION_NOTIFY(ABaseContainer, UniqInventoryes, ELifetimeCondition::COND_None, REPNOTIFY_Always);
}

void ABaseContainer::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);
}

bool ABaseContainer::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
    if (RelewantFor.Num() == 0)
    {
    	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
    }
    if (RelewantFor.Contains(RealViewer))
    {
    	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
    }
    return false;
}

void ABaseContainer::OnRep_Inventoryes()
{
    // OnUpdateContainer.Broadcast(this);
}

#pragma endregion REPLICATE

#pragma region GENERATE

void ABaseContainer::GenereteItems(APlayerController* For)
{
    if (LootComponent->GetRespawnLootType() == EGenerationLootType::EGLT_None) return;
    
    UBaseLineInventory* CurrentInventory = FindOrAdd(For);
    if (!CurrentInventory) return;    
    
    CurrentInventory->OnLeaveItem.RemoveDynamic(this, &ABaseContainer::UniquePickuped);
    CurrentInventory->ClearInventory();
    for (auto LootItem : LootComponent->GenereteItems(For))
    {
        CurrentInventory->AddTryItem(LootItem, true, false);
    }
    CurrentInventory->OnLeaveItem.AddUniqueDynamic(this, &ABaseContainer::UniquePickuped);
}


void ABaseContainer::UniquePickuped(UBaseLineInventory* From, UBaseItem* Item, bool IsLeaveItem)
{
    if (!IsLeaveItem) return;
    if (!(IsValid(From) && IsValid(From->GetOwner()))) return;

    if (Inventory == From)
    {
        SavedData = From->GetSaveData();
    }

    APlayerController* PC = (IsNetMode(NM_Standalone) ? GetWorld()->GetFirstPlayerController() : (Cast<APlayerController>(From->GetOwner())));
    if (!PC) return;
    
    LootComponent->SaveDataFor(PC, From->GetSaveData());
}

#pragma endregion GENERATE

#pragma region OVERLAP

void ABaseContainer::Sphere_BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Overlaped.Contains(OtherActor)) return;
	
    APawn* Character = Cast<APawn>(OtherActor);
    if (!Character) return;
    APlayerController* PlayerController = (Character != nullptr ? Cast<APlayerController>(Character->GetController()): nullptr);
    if (!PlayerController) return;
    
    FTimespan Result = (Date_RespawnLoot - FDateTime::UtcNow());
    // if (!CurrentGenerateData.bIsUniqueLoot && Result.GetTotalSeconds() > 0)
    // {
    // 	return;
    // }
	
    if (!Character->GetController())
    {
        FTimerDelegate Delegate;
        FTimerHandle Timer;
        TSoftObjectPtr<ACharacter> CharacterRef = Character;
        Delegate.BindUFunction(this, TEXT("AwaitPosses"), CharacterRef);
        GetWorld()->GetTimerManager().SetTimer(Timer, Delegate, 3.f, false);	
        return;
    }
	
	
    Overlaped.AddUnique(Character);
    GenereteItems(PlayerController);
}

void ABaseContainer::AwaitPosses(TSoftObjectPtr<ACharacter> Character)
{
    if (!IsPossiblyAllocatedUObjectPointer(Character.Get())) return;

    if (!IsOverlappingActor(Character.Get()))
    {
        return;
    }
	
    APlayerController* PlayerController = (Character.Get() != nullptr ? Cast<APlayerController>(Character.Get()->GetController()): nullptr);
    if (Character.Get()->GetController() && !PlayerController)
    {
        //It's AI
        return;
    }
    if (!PlayerController)
    {
        FTimerDelegate Delegate;
        FTimerHandle Timer;
        Delegate.BindUFunction(this, TEXT("AwaitPosses"), Character);
        GetWorld()->GetTimerManager().SetTimer(Timer, Delegate, 3.f, false);
        return;
    }
	
    if (Overlaped.Contains(Character)) return;

    Overlaped.AddUnique(Character.Get());
    GenereteItems(PlayerController);
}

void ABaseContainer::Sphere_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OverlappedActors.Contains(OtherActor)) return;
	
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!(IsValid(Character) && Cast<APlayerController>(Character->GetController()))) return;
	
    if (HasAuthority() && !OverlappedComp->IsOverlappingActor(Character))
    {
        OverlappedActors.Remove(Character);

        if (LootComponent->IsUniqueLoot())
        {
            RemoveInventoryByController(Character->GetController());
        }
    }
}

#pragma endregion OVERLAP

#pragma region TOOLS

void ABaseContainer::CheckInside()
{
    TArray<AActor*> LOverlappedActors;
    ContainerTrigger->GetOverlappingActors(LOverlappedActors);

    for (AActor*& LActor : LOverlappedActors)
    {
        APawn* LUser = Cast<APawn>(LActor);
        if (!IsValid(LUser)) continue;
        APlayerController* LPC = Cast<APlayerController>(LUser->GetController());
        if (!IsValid(LPC)) continue;
		
        GenereteItems(LPC);
        break;
    }
}

void ABaseContainer::MakeSphereTrigger()
{
    ContainerTrigger = NewObject<USphereComponent>(this);
    ContainerTrigger->RegisterComponentWithWorld(GetWorld());
    ContainerTrigger->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    ContainerTrigger->SetRelativeLocation(FVector(0, 0, 0));
    ContainerTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseContainer::Sphere_BeginOverlap);
    ContainerTrigger->OnComponentEndOverlap.AddUniqueDynamic(this, &ABaseContainer::Sphere_EndOverlap);

    ContainerTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    ContainerTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ContainerTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    ContainerTrigger->SetHiddenInGame(false);
    ContainerTrigger->SetVisibility(true);
    ContainerTrigger->SetSphereRadius(700.f, true);
    CheckInside();
}

UBaseLineInventory* ABaseContainer::GetInventoryByController(APlayerController* Controller)
{
    if (HasAuthority())
    {
        if (!LootComponent->IsUniqueLoot() || IsNetMode(ENetMode::NM_Standalone))
        {
            return Inventory;
        }
    }
    else
    {
        if (UniqInventoryes.Num() == 0)
        {
            return Inventory;
        }
		
        for (auto Result : UniqInventoryes)
        {
            if (!IsValid(Result)) continue;
            if (Result->GetOwner() == Controller) return Result;
        }
    }

    for (int32 Idx = 0; Idx < UniqInventoryes.Num(); Idx++)
    {
        if (IsValid(UniqInventoryes[Idx]) && IsValid(UniqInventoryes[Idx]->GetOwner())) continue;
        UniqInventoryes.RemoveAt(Idx);
        Idx--;
    }

    for (auto Result : UniqInventoryes)
    {
        if (!IsValid(Result)) continue;
        if (Result->GetOwner() == Controller) return Result;
    }
	
    return nullptr;
}

UBaseLineInventory* ABaseContainer::MakeInvetoryFor(APlayerController* Ref)
{
    if (HasAuthority() && IsPossiblyAllocatedUObjectPointer(Ref) && IsValid(Ref))
    {
        UBaseLineInventory* Result = NewObject<UBaseLineInventory>(Ref, ClassToInventory);
        Result->RegisterComponentWithWorld(GetWorld());
        Result->OnLeaveItem.AddUniqueDynamic(this, &ABaseContainer::UniquePickuped);
        UniqInventoryes.Add(Result);
        // OnRepDataInventoryes.Broadcast(this);
        TArray<UBaseItem*> LResult;
        Inventory->SetSize(FIntPoint(8, 20), LResult);
        ForceNetUpdate();
        return Result;
    }
    return nullptr;
}

UBaseLineInventory* ABaseContainer::MakeContainerInventory(bool WithCheck)
{
    if (IsValid(Inventory)) return Inventory;
		
    Inventory = NewObject<UBaseLineInventory>(this, ClassToInventory);
	
    Inventory->RegisterComponentWithWorld(GetWorld());
    if (WithCheck)
    {
        GetWorld()->GetTimerManager().SetTimer(Timer_CheckInventory, this, &ABaseContainer::CheckInventory, 30.f, false);
    }
    TArray<UBaseItem*> Result;
    Inventory->SetSize(FIntPoint(8, 20), Result);;
    Inventory->OnLeaveItem.AddUniqueDynamic(this, &ABaseContainer::UniquePickuped);
    ForceNetUpdate();		
    return Inventory;
}

UBaseLineInventory* ABaseContainer::FindOrAdd(APlayerController* Ref)
{
    UBaseLineInventory* Result = GetInventoryByController(Ref);
    if (IsValid(Result)) return Result;

    return (LootComponent->IsUniqueLoot() ? MakeInvetoryFor(Ref) : MakeContainerInventory(true));
}

void ABaseContainer::RemoveInventoryByController(AController *InOwner)
{
    if (!(LootComponent->IsUniqueLoot() && InOwner)) return;

    for (int32 Idx = 0; Idx < UniqInventoryes.Num(); Idx++)
    {
        if (!IsValid(UniqInventoryes[Idx]))
        {
            UniqInventoryes.RemoveAt(Idx);
            Idx--;
            continue;
        }
		
        if (UniqInventoryes[Idx]->GetOwner() != InOwner) continue;

        LootComponent->SaveDataFor(InOwner, UniqInventoryes[Idx]->GetSaveData());
        
        UniqInventoryes[Idx]->ClearInventory();
        UniqInventoryes[Idx]->SetIsReplicated(false);
		
        if (UniqInventoryes[Idx]->IsRegistered())
        {
            UniqInventoryes[Idx]->UnregisterComponent();
        }

        UniqInventoryes[Idx]->MarkAsGarbage();
        UniqInventoryes.RemoveAt(Idx);
        Idx--;
    }
}

void ABaseContainer::CheckInventory()
{
    if (!IsValid(Inventory)) return;
    if (!(IsValid(GetWorld()) && IsValid(GetWorld()->GetGameState()))) return;

    for (auto PS : GetWorld()->GetGameState()->PlayerArray)
    {
        if (!IsValid(PS->GetPawn())) continue;
        if (FVector::Distance(GetActorLocation(), PS->GetPawn()->GetActorLocation()) < 2000.f)
        {
            GetWorld()->GetTimerManager().SetTimer(Timer_CheckInventory, this, &ABaseContainer::CheckInventory, 30.f, false);
            return;
        }
    }

    SavedData = Inventory->GetSaveData();
    Inventory->ClearInventory();
    Inventory->SetIsReplicated(false);
    Inventory->MarkAsGarbage();
    Inventory = nullptr;
}

#pragma endregion
