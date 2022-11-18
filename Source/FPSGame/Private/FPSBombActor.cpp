//Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBombActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AFPSBombActor::AFPSBombActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	this->RootComponent = MeshComponent;

	CountDown = 2.0f;
}

// Called when the game starts or when spawned
void AFPSBombActor::BeginPlay()
{
	Super::BeginPlay();

	MaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (MaterialInstance)
	{
		CurrentColor = MaterialInstance->K2_GetVectorParameterValue("Color");
	}
	TargetColor = FLinearColor::MakeRandomColor();

	FTimerHandle Explosion_TimeHandler;
	GetWorldTimerManager().SetTimer(Explosion_TimeHandler, this, &AFPSBombActor::Explode, CountDown);

	
}

// Called every frame
void AFPSBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Progress = (GetWorld()->GetTimeSeconds() - CreationTime) / CountDown;

	const FLinearColor NewColor = FLinearColor::LerpUsingHSV(CurrentColor, TargetColor, Progress);
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Color", NewColor);
	}
}

void AFPSBombActor::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionTemplate, this->GetActorLocation());

	TArray<FOverlapResult> Overlaps;

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(500.0f);

	GetWorld()->OverlapMultiByObjectType(Overlaps, this->GetActorLocation(), FQuat::Identity, QueryParams, CollisionShape);

	for(FOverlapResult OverlapResult: Overlaps)
	{
		UPrimitiveComponent* Overlap = OverlapResult.GetComponent();
		if(Overlap && Overlap->IsSimulatingPhysics())
		{
			UMaterialInstanceDynamic* OverlapMaterialInstance = Overlap->CreateAndSetMaterialInstanceDynamic(0);
			if(OverlapMaterialInstance)
			{
				OverlapMaterialInstance->SetVectorParameterValue("Color", TargetColor);
			}
		}
	}
	Destroy();
}