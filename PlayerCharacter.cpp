// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to not call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0, 0, 70));
	//SpringArm->SetRelativeRotation(FRotator(0, -20, 0));
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation(FVector(0, 0, 30));
	FollowCamera->SetRelativeRotation(FRotator(-10, 0, 0));

	//Set the actors to seek from the collision channel
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLockedOn)
	{
		//Check if we are still in lock on distance
		//If not then unlock us on the next tick
		if (outHitResults.Num() > 0 && UKismetMathLibrary::Vector_Distance(GetActorLocation(), outHitResults[CurrentLockOnIndex].GetActor()->GetActorLocation()) > 3500)
		{
			bIsLockedOn = false;
			SetActorTickEnabled(false);
		}		

		//If there is an actor to lock on to it
		if (!outHitResults.IsEmpty())
		{
			//Lock the camera direction to the target
			GetController()->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), outHitResults[CurrentLockOnIndex].GetActor()->GetActorLocation()));
			
			//Get the roation that our character needs to face
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), outHitResults[CurrentLockOnIndex].GetActor()->GetActorLocation());
			
			//Change the yaw based on what we are looking at
			SetActorRotation(UE::Math::TRotator<double>(GetActorRotation().Roll, LookAtRotation.Yaw, GetActorRotation().Pitch));
		}
	}
	else
	{
		SetActorTickEnabled(true);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/*Binds the jump action*/
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		/*Binds the move action*/
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		/*Binds the look action*/
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		/*Binds the dodge action*/
		EnhancedInput->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dodge);

		/*Binds the lock on action*/
		EnhancedInput->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LockOn);

		/*Binds the display attacks right*/
		EnhancedInput->BindAction(DisplayAttacksRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DisplayRightAttacks);
		EnhancedInput->BindAction(DisplayAttacksRightAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetMenuOneFalse);

		/*Binds the display attacks left*/
		EnhancedInput->BindAction(DisplayAttacksLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DisplayLeftAttacks);
		EnhancedInput->BindAction(DisplayAttacksLeftAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetMenuTwoFalse);

		/*Bind the interact button*/
		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		/*Bind the use item button*/
		EnhancedInput->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &APlayerCharacter::UseItem);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
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

void APlayerCharacter::Look(const FInputActionValue& Value)
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

//Called when the character dodges
void APlayerCharacter::Dodge(const FInputActionValue& Value)
{

	//If we are allowed to dodge and we are not falling and no montage is playing
	if (bCanDodge && !GetCharacterMovement()->IsFalling() && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(NULL))
	{
		bCanDodge = false;

		//If we have movement do a directional roll.
		//Otherwise do a backstep.
		if (GetCharacterMovement()->GetLastUpdateVelocity().Length() > 0)
		{
			//Play the roll animation
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			SetActorRotation(GetDodgeRotation());
			AnimInstance->Montage_Play(DodgeMontage, 1);
		}
		else
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			//Play the backstep animation
			AnimInstance->Montage_Play(BackstepMontage, 1);
		}

		FTimerHandle DodgeTimer;
		GetWorldTimerManager().SetTimer(DodgeTimer, this, &APlayerCharacter::ResetDodge, 0.5f, false, 0.5f);
	}
}

void APlayerCharacter::ResetDodge()
{
	bCanDodge = true;
}

FRotator APlayerCharacter::GetDodgeRotation()
{
	if (Controller != nullptr)
	{
		if(GetVelocity().Length() > 0)
			return GetPendingMovementInputVector().Rotation();
	}
	return GetActorRotation();
}

void APlayerCharacter::LockOn(const FInputActionValue& Value)
{
	//Sets a timer so we can't spam the lock on
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, 0.1f, false);

	//If we are not locked on to a target try and lock on to something
	//Otherwise unlock 
	if (!bIsLockedOn)
	{
		//Enable tick events
		SetActorTickEnabled(true);

		//Start location of the sphere trace is the player location
		FVector StartLocation = GetActorLocation() + FVector(0, 0, 300);
		

		//Get the forward vector of the camera (the start locaiton of the sphere trace)
		FVector CameraForwardVector = FollowCamera->GetForwardVector();

		//Set the end location of the sphere trace
		FVector EndLocation = StartLocation + (CameraForwardVector * 1000);

		//Ignore any specific actors
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Init(this, 1);
		
		//Draw the sphere trace and get the valid pawns it finds
		UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), 
														StartLocation, 
														EndLocation, 
														1000, 
														TraceObjectTypes,
														false, 
														ActorsToIgnore,
														EDrawDebugTrace::None, 
														outHitResults, 
														true);
		bIsLockedOn = true;
	}
	else
	{
		//Set locked on to false and remove all elements from the hit results array
		bIsLockedOn = false;
		outHitResults.Empty();
	}	
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacting"));
}

void APlayerCharacter::UseItem(const FInputActionValue& Value)
{
	bCanDodge = false;
	UE_LOG(LogTemp, Warning, TEXT("Using item"));
}

void APlayerCharacter::DisplayRightAttacks(const FInputActionValue& Value)
{
	bCanDodge = false;
	bMenuOneIsUp = true;
	UE_LOG(LogTemp, Warning, TEXT("Holding the right trigger"));
}

void APlayerCharacter::DisplayLeftAttacks(const FInputActionValue& Value)
{
	bMenuTwoIsUp = true;
	UE_LOG(LogTemp, Warning, TEXT("Holding the left trigger"));
}

/*Picks the right face attack from the attack menu*/
void APlayerCharacter::RightAttack(const FInputActionValue& Value)
{
	
}

/*Picks the up face attack from the attack menu*/
void APlayerCharacter::UpAttack(const FInputActionValue& Value)
{
	
}

/*Picks the left face attack from the attack menu*/
void APlayerCharacter::LeftAttack(const FInputActionValue& Value)
{
	
}

/*Picks the down face attack from the attack menu*/
void APlayerCharacter::DownAttack(const FInputActionValue& Value)
{
	
}

void APlayerCharacter::SetMenuOneFalse()
{
	UE_LOG(LogTemp, Warning, TEXT("Set the menu one variable to false"));
	bMenuOneIsUp = false;
	bCanDodge = true;
}

void APlayerCharacter::SetMenuTwoFalse()
{
	UE_LOG(LogTemp, Warning, TEXT("Set the menu two variable to false"));
	bMenuTwoIsUp = false;
	bCanDodge = true;
}

void APlayerCharacter::SetMenuThreeFalse()
{
	bMenuThreeIsUp = false;
	bCanDodge = true;
}