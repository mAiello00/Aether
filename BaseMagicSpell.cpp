#include "BaseMagicSpell.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABaseMagicSpell::ABaseMagicSpell()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		if (!CollisionComponent)
		{
			//Use a sphere as a simple collision respreseentation
			CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphere Component"));

			//Set the sphere collision radius
			CollisionComponent->InitSphereRadius(15.0f);

			//Set the root component to be the collision component
			RootComponent = CollisionComponent;
		}
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
}

// Called when the game starts or when spawned
void ABaseMagicSpell::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseMagicSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseMagicSpell::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
