// Fill out your copyright notice in the Description page of Project Settings.

#include "PositionReport.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UPositionReport::UPositionReport()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UPositionReport::BeginPlay()
{
	Super::BeginPlay();
	FString ObjectName = GetOwner()->GetName();
	FVector ObjectLocation = GetOwner()->GetActorLocation();
	FVector ObjTransformLocation = GetOwner()->GetTransform().GetLocation();

	FString ObjectLocationReport = *FString::Printf(TEXT("X=%f,Y=%f,Z=%f"), ObjectLocation.X, ObjectLocation.Y, ObjectLocation.Z);
	FString ObjectTransformLocationReport = *FString::Printf(TEXT("X=%f,Y=%f,Z=%f"), ObjTransformLocation.X, ObjTransformLocation.Y, ObjTransformLocation.Z);
	UE_LOG(LogTemp, Display, TEXT("Position report begin play in %s. Object location = %s"), *ObjectName, *ObjectLocationReport);
	UE_LOG(LogTemp, Display, TEXT("Object transform location = %s"), *ObjectTransformLocationReport);
	// ...
	
}


// Called every frame
void UPositionReport::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

