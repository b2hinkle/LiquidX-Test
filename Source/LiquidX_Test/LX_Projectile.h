// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LX_Projectile.generated.h"

UCLASS()
class LIQUIDX_TEST_API ALX_Projectile : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient)
		TWeakObjectPtr<APawn> PawnToChase;

	ALX_Projectile();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* ProjectileMesh;
	UPROPERTY()
		TObjectPtr<USceneComponent> SceneComponent;
	// Distance to travel per cm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float Speed;

	UFUNCTION()
		void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
