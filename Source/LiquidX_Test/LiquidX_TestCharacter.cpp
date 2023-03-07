// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidX_TestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


#include "Utilities/LX_CollisionChannels.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LX_Projectile.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

float ALiquidX_TestCharacter::CharacterFOV = 150;

ALiquidX_TestCharacter::ALiquidX_TestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ShootTargetingDistance = 10000;
	ShootTargetingSphereRadius = 50;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALiquidX_TestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALiquidX_TestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidX_TestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALiquidX_TestCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ALiquidX_TestCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ALiquidX_TestCharacter::StopShoot);

		//Backstab
		EnhancedInputComponent->BindAction(BackstabAction, ETriggerEvent::Triggered, this, &ALiquidX_TestCharacter::Backstab);
		EnhancedInputComponent->BindAction(BackstabAction, ETriggerEvent::Completed, this, &ALiquidX_TestCharacter::StopBackstab);
	}

}

void ALiquidX_TestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALiquidX_TestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ALiquidX_TestCharacter::Shoot()
{
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation + (GetControlRotation().Vector() * ShootTargetingDistance);

	FHitResult OutHit;
	const bool bBlockingHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, ShootTargetingSphereRadius, UEngineTypes::ConvertToTraceType(COLLISIONCHANNEL_SHOOT), false, { this }, EDrawDebugTrace::ForOneFrame, OutHit, true);

}
void ALiquidX_TestCharacter::StopShoot()
{
	ServerStopShoot();
}

void ALiquidX_TestCharacter::ServerStopShoot_Implementation()
{
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation + (GetControlRotation().Vector() * ShootTargetingDistance);
	FHitResult OutHit;
	const bool bBlockingHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, ShootTargetingSphereRadius, UEngineTypes::ConvertToTraceType(COLLISIONCHANNEL_SHOOT), false, { this }, EDrawDebugTrace::ForOneFrame, OutHit, true);
	if (bBlockingHit)
	{
		const AActor* HitActor = OutHit.GetActor();
		if (IsValid(HitActor) && HitActor->IsA<APawn>())
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ALX_Projectile* SpawnedProjectile = GetWorld()->SpawnActor<ALX_Projectile>(ProjectileClass, GetActorTransform(), SpawnParameters);

			if (IsValid(SpawnedProjectile))
			{
				if (APawn* HitPawn = Cast<APawn>(OutHit.GetActor()))
				{
					SpawnedProjectile->PawnToChase = HitPawn;
				}
			}
		}
	}
}

void ALiquidX_TestCharacter::Backstab()
{
	TArray<FOverlapResult> OverlapResults;
	const FVector BoxHalfExtent = FVector(3, 25, 50);
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = GetActorLocation() + (GetActorForwardVector() * 50);
	FHitResult OutHit = FHitResult();
	UKismetSystemLibrary::BoxTraceSingle(this, TraceStart, TraceEnd, BoxHalfExtent, GetActorRotation(), UEngineTypes::ConvertToTraceType(COLLISIONCHANNEL_SHOOT), false, { this }, EDrawDebugTrace::ForOneFrame, OutHit, true);
}
void ALiquidX_TestCharacter::StopBackstab()
{
	ServerStopBackstab();
}

void ALiquidX_TestCharacter::ServerStopBackstab_Implementation()
{
	TArray<FOverlapResult> OverlapResults;
	const FVector BoxHalfExtent = FVector(3, 25, 50);
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = GetActorLocation() + (GetActorForwardVector() * 50);
	FHitResult OutHit = FHitResult();
	UKismetSystemLibrary::BoxTraceSingle(this, TraceStart, TraceEnd, BoxHalfExtent, GetActorRotation(), UEngineTypes::ConvertToTraceType(COLLISIONCHANNEL_SHOOT), false, { this }, EDrawDebugTrace::ForOneFrame, OutHit, true);
	

	AActor* Actor = OutHit.GetActor();
	if (IsValid(Actor) && Actor->IsA<APawn>() && Actor != this)
	{
		if (CanBackstab(this, Actor, CharacterFOV))
		{
			Actor->Destroy();
		}
	}
}

bool ALiquidX_TestCharacter::CanBackstab(const AActor* A, const AActor* B, const float ThresholdAngle)
{
	const bool ASeesB = IsInFOV(A, B, ThresholdAngle);
	const bool BSeesA = IsInFOV(B, A, ThresholdAngle);
	return ASeesB && !BSeesA;
}

bool ALiquidX_TestCharacter::IsInFOV(const AActor* A, const AActor* B, const float ThresholdAngle)
{
	const FVector AFwd = A->GetActorForwardVector();
	const FVector AToB = (B->GetActorLocation() - A->GetActorLocation()).GetSafeNormal(); // normalized vectors allow us to skip a step when obtaining the angle
	const float Dot = FVector::DotProduct(AFwd, AToB);
	const float RadianAngle = FMath::Acos(Dot);
	const float DegreeAngle = RadianAngle * (180/PI); // convert from radians

	return DegreeAngle <= ThresholdAngle;
}

void ALiquidX_TestCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleSystem, GetActorTransform());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeathSound, GetActorLocation(), GetActorRotation(), .1);
	}
}
