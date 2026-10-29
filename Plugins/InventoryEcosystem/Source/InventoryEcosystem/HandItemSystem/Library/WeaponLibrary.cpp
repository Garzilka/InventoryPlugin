#include "WeaponLibrary.h"

#include "InventoryEcosystem/HandItemSystem/Data/BalisticFXData.h"
#include "Kismet/GameplayStatics.h"


void UWeaponLibrary::BalisticFX(AActor* Instigator, FHitResult IntHit, class UBalisticFXDataTable* BalisticFXData)
{
	if (!BalisticFXData) return;

	TEnumAsByte<EPhysicalSurface> LSurfaceType = UGameplayStatics::GetSurfaceType(IntHit);
	FBalisticVFXData* LBalisticData = BalisticFXData->BalisticData.Find(LSurfaceType);
	if (LBalisticData == nullptr) return;

	if (LBalisticData->Decals.IsValidIndex(0))
	{
		FDecalBalisticData DecalBalistic = LBalisticData->Decals[FMath::RandRange(0, LBalisticData->Decals.Num() - 1)];
		if (DecalBalistic.BulitDecal)
		{
			UGameplayStatics::SpawnDecalAtLocation(Instigator->GetWorld(), DecalBalistic.BulitDecal, DecalBalistic.SizeDecale, IntHit.ImpactPoint, IntHit.ImpactNormal.Rotation(), DecalBalistic.LifeTime);
		}

		if (UKismetSystemLibrary::IsValidClass(DecalBalistic.BlueprintDecalClass))
		{
			FTransform LSpwanTransfor(IntHit.ImpactNormal.Rotation(), IntHit.ImpactPoint);
			AActor* ActorDelac = Instigator->GetWorld()->SpawnActor<AActor>(DecalBalistic.BlueprintDecalClass, LSpwanTransfor);
			ActorDelac->SetLifeSpan(DecalBalistic.LifeTime);
		}
	}


	if (LBalisticData->ImpactNiagara.IsValidIndex(0))
	{
		int32 RandIdx = FMath::RandRange(0, LBalisticData->ImpactNiagara.Num() - 1);
		
		if (LBalisticData->ImpactNiagara.IsValidIndex(RandIdx) && LBalisticData->ImpactNiagara[RandIdx])
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Instigator->GetWorld(), LBalisticData->ImpactNiagara[RandIdx], IntHit.ImpactPoint, (IntHit.ImpactNormal.Rotation() + FRotator(-90.0f, 0.0f, 0.0f)));
		}
			
	} 
	else if (LBalisticData->ImpactEmitter.IsValidIndex(0))
	{
		int32 RandIdx = FMath::RandRange(0, LBalisticData->ImpactEmitter.Num() - 1);
		if (LBalisticData->ImpactEmitter.IsValidIndex(RandIdx) && LBalisticData->ImpactEmitter[RandIdx])
		{
			UGameplayStatics::SpawnEmitterAtLocation(Instigator->GetWorld(), LBalisticData->ImpactEmitter[RandIdx], FTransform(IntHit.ImpactNormal.Rotation() + FRotator(-90.0f, 0.0f, 0.0f), IntHit.ImpactPoint));
		}
	}

	if (LBalisticData->ImpactSound)
		UGameplayStatics::SpawnSoundAtLocation(Instigator->GetWorld(), LBalisticData->ImpactSound, IntHit.Location);
}
