// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class AETHER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DisplayAttacksRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DisplayAttacksLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* UseItemAction;

	//Dodge montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DodgeMontage;

	//Backstep montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* BackstepMontage;


public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called for the move input */
	void Move(const FInputActionValue& Value);

	/*Called for the look input*/
	void Look(const FInputActionValue& Value);

	/*Called for the dodge action*/
	void Dodge(const FInputActionValue& Value);

	/*Called for the dodge action*/
	void LockOn(const FInputActionValue& Value);

	/*Right face button attack when holding right trigger*/
	void RightAttack(const FInputActionValue& Value);

	/*Up face button attack when holding right trigger*/
	void UpAttack(const FInputActionValue& Value);

	/*Left face button attack when holding right trigger*/
	void LeftAttack(const FInputActionValue& Value);

	/*Down face button attack when holding right trigger*/
	void DownAttack(const FInputActionValue& Value);

	/*Holding right trigger to display different set of attacks*/
	void DisplayRightAttacks(const FInputActionValue& Value);

	/*Holding left trigger to display different set fo attacks*/
	void DisplayLeftAttacks(const FInputActionValue& Value);

	/*Interact with world*/
	void Interact(const FInputActionValue& Value);

	/*Called when using an item*/
	void UseItem(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	// Lock on 
	/*-------------------------------------*/

	//Is out character currently locked on to a target
	bool bIsLockedOn = false;

	//Hit actors from the sphere trace
	TArray<FHitResult> outHitResults;

	//Array that holds the actors to seek during lock-on
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	//Index for the outHitResult array
	int CurrentLockOnIndex = 0;

	//Dodge
	/*-------------------------------------*/
	
	//Is the character currently allowed to dodge
	bool bCanDodge = true;

	//Resets the value of bCanDodge to true
	void ResetDodge();

	//Gets the rotation we want to dodge in
	FRotator GetDodgeRotation();

	//Spell Casting
	/*-------------------------------------*/

	//Boolean to tell us if we are currently casting a spell
	bool bIsCastingSpell = false;

	//Booleans for the attack menus and functions to reset them
	/*-------------------------------------*/
	bool bMenuOneIsUp = false;
	void SetMenuOneFalse();
	bool bMenuTwoIsUp = false;
	void SetMenuTwoFalse();
	bool bMenuThreeIsUp = false;
	void SetMenuThreeFalse();

};
