#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BaseMagicSpell.generated.h"

UCLASS()
class AETHER_API ABaseMagicSpell : public AActor
{
	GENERATED_BODY()

	//Sphere collision component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;

	//Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
public:
	// Sets default values for this actor's properties
	ABaseMagicSpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Default mana cost
	UPROPERTY(EditAnywhere)
	int ManaCost = 100;

	//Default spell range
	UPROPERTY(EditAnywhere)
	float DefaultSpellRange = 500;

	//Default spell damage
	UPROPERTY(EditAnywhere)
	float SpellDamage = 0;

	//Function that initializes the projectiles velocity in the firing direction
	void FireInDirection(const FVector& ShootDirection);
};
