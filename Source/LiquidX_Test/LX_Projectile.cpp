// Fill out your copyright notice in the Description page of Project Settings.


#include "LX_Projectile.h"

#include "LiquidX_TestCharacter.h"

ALX_Projectile::ALX_Projectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SetRootComponent(SceneComponent);
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	ProjectileMesh->SetupAttachment(RootComponent);

	Speed = 150;
}

void ALX_Projectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ALX_Projectile::OnProjectileBeginOverlap);
}

void ALX_Projectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		return;
	}

	if (IsValid(OtherActor) && OtherActor != GetOwner())
	{
		ALiquidX_TestCharacter* LXTest_Character = Cast<ALiquidX_TestCharacter>(OtherActor);
		if (IsValid(LXTest_Character))
		{
			LXTest_Character->Destroy();
		}
		Destroy();
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
			const FVector ThisActorLocation = GetActorLocation();
			const FVector PawnToChaseActorLocation = PawnToChase->GetActorLocation();
			const FVector NewLocation = (PawnToChaseActorLocation - ThisActorLocation).GetSafeNormal() * Speed * DeltaTime;
			AddActorWorldOffset(NewLocation);
		}
		else
		{
			AddActorLocalOffset(GetActorForwardVector() * Speed * DeltaTime);
		}
	}

}

