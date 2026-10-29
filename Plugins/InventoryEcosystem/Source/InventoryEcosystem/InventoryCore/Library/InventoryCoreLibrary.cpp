// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryCoreLibrary.h"

#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/InventoryCore/Object/CoreInventoryObject.h"
#include "GameFramework/PlayerState.h"
#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LOG_IES_BIO, All, All);
static TAutoConsoleVariable<bool> C_IES_VIEW_LOG_BIO(TEXT("IES.LOG.BIO"), false, TEXT("Enable write log to screen"), ECVF_Cheat);

IItemManagmentInterface* UInventoryCoreLibrary::HasManagmentInterface(UObject* TargetObject)
{
	if (TargetObject == nullptr) return nullptr;
	if (UKismetSystemLibrary::DoesImplementInterface(TargetObject, UItemManagmentInterface::StaticClass()))
	{
		return Cast<IItemManagmentInterface>(TargetObject);
	}
	return nullptr;
}

IInventoryMeshInterface * UInventoryCoreLibrary::HasMeshInterface(UObject *TargetObject)
{
    if (TargetObject == nullptr) return nullptr;
    if (UKismetSystemLibrary::DoesImplementInterface(TargetObject, UInventoryMeshInterface::StaticClass()))
    {
        return Cast<IInventoryMeshInterface>(TargetObject);
    }
    return nullptr;
}

USkeletalMeshComponent* UInventoryCoreLibrary::GetInventoryMesh(AActor* From)
{
	auto LInterface = UInventoryCoreLibrary::HasMeshInterface(From);
	if (!LInterface) return nullptr;
	return LInterface->Execute_I_GetMesh(From);
}

void UInventoryCoreLibrary::PrintString(FString Message, FColor Color)
{
	if (!GEngine) return;
	GEngine->AddOnScreenDebugMessage(-1, 8, Color, Message);
}

void UInventoryCoreLibrary::PrintBIO_LOG(AActor* Owner, UCoreInventoryObject* InObject, E_IES_LOG_TYPE TypeLog, FString Str,
                                         int Line, const char* Function)
{
	if (Owner == nullptr || InObject == nullptr) return;

	if (C_IES_VIEW_LOG_BIO.GetValueOnGameThread())
	{
		UInventoryCoreLibrary::PrintString(Str, UInventoryCoreLibrary::ErrorToColor(TypeLog));
	}

	switch (TypeLog)
	{
	case E_IES_LOG_TYPE::EIEST_Display:
		{
			UE_LOG(LOG_IES_BIO, Display, TEXT("[%s][%i] | Net mode: %s | Player name: %s | Item name: %s | Text: %s"),
				ANSI_TO_TCHAR(Function), Line, *UInventoryCoreLibrary::GetNetMode(Owner), *UInventoryCoreLibrary::GetPlayerNickname(Owner), *(InObject->GetClass()->GetName()), *Str);
			break;
		}
	case E_IES_LOG_TYPE::EIEST_Warning:
		UE_LOG(LOG_IES_BIO, Warning, TEXT("[%s][%i] | Net mode: %s | Player name: %s | Item name: %s | Text: %s"),
			ANSI_TO_TCHAR(Function), Line, *UInventoryCoreLibrary::GetNetMode(Owner), *UInventoryCoreLibrary::GetPlayerNickname(Owner), *(InObject->GetClass()->GetName()), *Str);
		break;
	case E_IES_LOG_TYPE::EIEST_Error:
		UE_LOG(LOG_IES_BIO, Error, TEXT("[%s][%i] | Net mode: %s | Player name: %s | Item name: %s | Text: %s"),
			ANSI_TO_TCHAR(Function), Line, *UInventoryCoreLibrary::GetNetMode(Owner), *UInventoryCoreLibrary::GetPlayerNickname(Owner), *(InObject->GetClass()->GetName()), *Str);
		break;
	default:
		break;
	}
}

FColor UInventoryCoreLibrary::ErrorToColor(E_IES_LOG_TYPE ErrorType)
{
	switch (ErrorType)
	{
	case E_IES_LOG_TYPE::EIEST_Display:	return FColor::White;
	case E_IES_LOG_TYPE::EIEST_Warning:	return FColor::Orange;
	case E_IES_LOG_TYPE::EIEST_Error:	return FColor::Red;
	default:								return FColor();
	}
}

FString UInventoryCoreLibrary::GetNetMode(AActor* InActor)
{
	if (!InActor) return FString("nullptr");

	const ENetMode LNetMode = InActor->GetNetMode();

	switch (LNetMode)
	{
	case NM_Standalone:			return FString("NM_Standalone");
	case NM_DedicatedServer:	return FString("NM_DedicatedServer");
	case NM_Client:				return FString("NM_Client");
	case NM_ListenServer:		return FString("NM_ListenServer");
	default: return FString("nullptr");
	}
}

FString UInventoryCoreLibrary::GetPlayerNickname(AActor* From)
{
	if (!From) return "";
	APlayerState* LPlayerState = UInventoryCoreLibrary::GetPlayerState(From);
	if (!LPlayerState) return From->GetName();
	return LPlayerState->GetPlayerName();
}

APlayerState* UInventoryCoreLibrary::GetPlayerState(AActor* Owner)
{
	return (Cast<ACharacter>(Owner) != nullptr ? GetPlayerState(Cast<ACharacter>(Owner)) : GetPlayerState(Cast<APlayerController>(Owner)));
}

APlayerState* UInventoryCoreLibrary::GetPlayerState(ACharacter* Owner)
{
	if (!Owner) return nullptr;
	APlayerState* LResult = Owner->GetPlayerState();
	if (LResult)
	{
		return LResult;
	}
	else if (Owner->GetController())
	{
		APlayerController* LPlayerController = Cast<APlayerController>(Owner->GetController());
		if (!LPlayerController) return LResult;
		return LPlayerController->GetPlayerState<APlayerState>();
	}
	return nullptr;
}

APlayerState* UInventoryCoreLibrary::GetPlayerState(APlayerController* Owner)
{
	if (!Owner) return nullptr;
	APlayerState* LResult = Owner->GetPlayerState<APlayerState>();
	if (LResult)
	{
		return LResult;
	}
	else if (Owner->GetPawn())
	{
		ACharacter* LPlayerCharacter = Cast<ACharacter>(Owner->GetPawn());
		if (!LPlayerCharacter) return LResult;
		return LPlayerCharacter->GetPlayerState();
	}
	return nullptr;
}