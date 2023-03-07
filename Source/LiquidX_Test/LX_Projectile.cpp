// Fill out your copyright notice in the Description page of Project Settings.


#include "LX_Projectile.h"

ALX_Projectile::ALX_Projectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SetRootComponent(SceneComponent);
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ProjectileMesh->SetupAttachment(RootComponent);

	Speed = 5;
}

void ALX_Projectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ALX_Projectile::OnDeadlyBoxBeginOverlap);
}

void ALX_Projectile::OnDeadlyBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (OtherActor->IsA<APawn>() && OtherActor != GetOwner()) 
		{
			OtherActor->Destroy();
			Destroy();
		}
	}
}

void ALX_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ProjectileMesh)
	{
		AddActorLocalRotation(FRotator(100, 100, 100));
		if (PawnToChase.IsValid())
		{
			FVector ThisActorLocation = GetActorLocation();
			FVector PawnToChaseActorLocation = PawnToChase->GetActorLocation();
			FVector NewLocation = (PawnToChaseActorLocation - ThisActorLocation).GetSafeNormal() * Speed;
			AddActorWorldOffset(NewLocation);
		}
		else
		{
			AddActorLocalOffset(GetActorForwardVector() * Speed);
		}
	}

}

