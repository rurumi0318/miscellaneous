// Fill out your copyright notice in the Description page of Project Settings.


#include "Follower.h"

// Sets default values
AFollower::AFollower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFollower::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("AFollower::BeginPlay"));
	InitSecondOrderDynamics();
	Super::BeginPlay();
}

void AFollower::OnConstruction(const FTransform& Transform)
{
	UE_LOG(LogTemp, Warning, TEXT("AFollower::OnConstruction"));
	InitSecondOrderDynamics();
	Super::OnConstruction(Transform);
}

void AFollower::InitSecondOrderDynamics()
{
	UE_LOG(LogTemp, Warning, TEXT("AFollower::InitSecondOrderDynamics"));
	SecondOrderDynamics = FSecondOrderDynamics(Frequency, Damping, Response, GetActorLocation());
}

// Called every frame
void AFollower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor == nullptr || IsValid(TargetActor) == false)
		return;

	FVector NewPosition = SecondOrderDynamics.Update(DeltaTime, TargetActor->GetActorLocation());
	SetActorLocation(NewPosition);
}

