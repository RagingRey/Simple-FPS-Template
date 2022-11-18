// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBombActor.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSBombActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFPSBombActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMaterialInstanceDynamic* MaterialInstance;
	FLinearColor CurrentColor;
	FLinearColor TargetColor;

	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		float CountDown;

	UPROPERTY(EditDefaultsOnly, Category = "BombActor")
		UParticleSystem* ExplosionTemplate;

	UFUNCTION()
		void Explode();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};