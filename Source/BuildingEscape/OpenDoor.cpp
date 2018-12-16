// Copyright Paul Klinker 2018

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/PrimitiveComponent.h"
#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	// Find the Owning actor
	Owner = GetOwner();
}



// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// poll the trigger volume
	if (GetTotalMassOfActorsOnPlate() >= TriggerMass)
	{
		// if the ActorThatOpens is the volume then open door
		OnOpen.Broadcast();

	} 
	else
	{
		OnClose.Broadcast();
	}


}

// Returns total mass in kilograms
float UOpenDoor::GetTotalMassOfActorsOnPlate() {
	float TotalMass = 0.f;
	// find all the overlapping actors
	TArray<AActor*> OverlappingActors;
	if (PressurePlate)
	{
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);
		// sum their masses
		for (auto& OverlappedActor : OverlappingActors)
		{
			if (OverlappedActor)
			{
				UPrimitiveComponent *PrimComponent = OverlappedActor->FindComponentByClass<UPrimitiveComponent>();
				if (PrimComponent)
				{
					float AMass = PrimComponent->GetMass();
					TotalMass = TotalMass + AMass;
					//UE_LOG(LogTemp, Warning, TEXT("The actor on the plate: %s, mass is %f"), *OverlappedActor->GetName(), TotalMass);
				}
					
			}
		}
	} 
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Null PressurePlate."));
	}
	return TotalMass;
}

