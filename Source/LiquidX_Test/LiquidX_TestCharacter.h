// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "LiquidX_TestCharacter.generated.h"


UCLASS(config=Game)
class ALiquidX_TestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackstabAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = CharacterVisuals)
		UParticleSystem* DeathParticleSystem;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = CharacterVisuals)
		USoundBase* DeathSound;

	/** Look Input Action */
	UPROPERTY(EditDefaultsOnly, Category = Shoot)
		TSubclassOf<AActor> ProjectileClass;

	/** How far the targeting reaches in cm */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shoot, meta = (AllowPrivateAccess = "true"))
		float ShootTargetingDistance;
	/** How large the targeting sphere is */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shoot, meta = (AllowPrivateAccess = "true"))
		float ShootTargetingSphereRadius;
	/** Angle representing character's view (from his/her foward, all the way to his/her peripheral vision). */
	static float CharacterFOV;

public:
	ALiquidX_TestCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Targeting event */
	void Shoot();
	/** Confirming event */
	void StopShoot();

	/** Gameplay logic  */
	UFUNCTION(Server, Reliable)
		void ServerStopShoot();

	/** Targeting event */
	void Backstab();
	/** Confirming event */
	void StopBackstab();
	UFUNCTION(Server, Reliable)
		void ServerStopBackstab();

	static bool CanBackstab(const AActor* A, const AActor* B, const float ThresholdAngle);
	static bool IsInFOV(const AActor* A, const AActor* B, const float ThresholdAngle);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

