// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBulit.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseBulit::ABaseBulit()
{
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(50);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Granade"));
	ProjectileMovement->SetAutoActivate(false);
	ProjectileMovement->SetIsReplicated(false);

	ProjectileCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Projectile Capsule"));
	RootComponent = ProjectileCapsule;
	ProjectileCapsule->bReturnMaterialOnMove = true;
	ProjectileCapsule->MoveIgnoreActors.Add(this);
	ProjectileCapsule->MoveIgnoreActors.Add(GetOwner());
	ProjectileCapsule->OnComponentHit.AddDynamic(this, &ABaseBulit::Hit);
}

void ABaseBulit::AddIgnoreActor(AActor* InActor) const
{
	ProjectileCapsule->MoveIgnoreActors.Add(InActor);
	ProjectileCapsule->IgnoreActorWhenMoving(InActor, true);
}

// Called when the game starts or when spawned
void ABaseBulit::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.0f);

	if (ACharacter* Player = Cast<ACharacter>(GetOwner()))
	{
		SetActorHiddenInGame(Player->IsLocallyControlled());
	}

}

void ABaseBulit::Launch(FVector InVelocity, float InDistance, TSubclassOf<UDamageType> InDamageType)
{
	//DamageType = InDamageType;
	ProjectileCapsule->MoveIgnoreActors.Add(this);
	if (GetOwner())
	{
		Start = GetActorLocation();
		Distance = InDistance;
		ProjectileCapsule->MoveIgnoreActors.Add(GetOwner());
		ProjectileCapsule->IgnoreActorWhenMoving(GetOwner(), true);
		if (GetOwner()->GetOwner())
		{
			ProjectileCapsule->MoveIgnoreActors.Add(GetOwner()->GetOwner());
			ProjectileCapsule->IgnoreActorWhenMoving(GetOwner()->GetOwner(), true);
		}
	}
	ProjectileMovement->Velocity = InVelocity;
	ProjectileMovement->ToggleActive();
}

void ABaseBulit::Hit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor != GetOwner())
	{
		if (ABaseBulit* Other = Cast<ABaseBulit>(OtherActor))
		{
			ProjectileCapsule->MoveIgnoreActors.Add(Other);
			if (GetOwner()->GetOwner())
			{
				ProjectileCapsule->MoveIgnoreActors.Add(Other);
				ProjectileCapsule->IgnoreActorWhenMoving(Other, true);
			}
			ProjectileCapsule->IgnoreActorWhenMoving(Other, true);
			return;
		}

		UWeaponLibrary::BalisticFX(this, Hit, BalisticFXData);

		AActor* LActor = OtherActor;
		if (LActor)
		{
			FString Ls = ("Damage: " + LActor->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, Ls);
			UE_LOG(LogTemp, Warning, TEXT("Bulit Damage: %s"), *Ls);
		}

		if (OtherActor && HasAuthority())
		{
			AActor* GetOwnerOwner = GetOwner();

			float ResultDamage = abs(Damage * (1 - abs(FVector::Distance(Start, GetActorLocation()) / Distance)));

			UGameplayStatics::ApplyPointDamage(OtherActor, ResultDamage, OtherActor->GetActorLocation(), Hit, GetOwner()->GetInstigatorController(), this, DamageType);

			UE_LOG(LogTemp, Warning, TEXT("Bulit Damage: %f"), ResultDamage);
		}
		SetActorHiddenInGame(true);
		this->Destroy();
	}
}