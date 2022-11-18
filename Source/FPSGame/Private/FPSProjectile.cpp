#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
}

void AFPSProjectile::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandler;
	GetWorldTimerManager().SetTimer(TimerHandler, this, &AFPSProjectile::Explode, DestructionTime, false);
}

void AFPSProjectile::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, this->GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f));

	BlueprintExplode();

	Destroy();
}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherComp->IsSimulatingPhysics()))
	{
		const float RandomIntensity = FMath::FRandRange(200.0f, 500.0f);
		OtherComp->AddImpulseAtLocation(this->GetVelocity() * RandomIntensity, this->GetActorLocation());

		FVector Scale = OtherComp->GetComponentScale();
		Scale *= 0.8;

		if(Scale.GetMin() <= 0.5f)
		{
			OtherActor->Destroy();
		}
		else
		{
			OtherComp->SetWorldScale3D(Scale);
		}

		UMaterialInstanceDynamic* OtherActorMaterial = OtherComp->CreateAndSetMaterialInstanceDynamic(0);
		if(OtherActorMaterial)
		{
			OtherActorMaterial->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}

		Explode();
	}
}