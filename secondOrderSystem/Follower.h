// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SecondOrderDynamics.h"
#include "GameFramework/Actor.h"
#include "Follower.generated.h"

UCLASS()
class TESTEVERYTHING_API AFollower : public AActor
{
	GENERATED_BODY()

public:

	AFollower();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Frequency = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damping = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Response = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* TargetActor = nullptr;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	FSecondOrderDynamics SecondOrderDynamics;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void InitSecondOrderDynamics();
};
