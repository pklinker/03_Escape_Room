// Copyright Paul Klinker 2018

#include "Grabber.h"
#include "GameFramework/Actor.h" // for the GetOwner call
#include "Engine/Player.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT // used to remind that the parameter is an out parameter

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FString ObjectName = GetOwner()->GetName();
	//UE_LOG(LogTemp, Display, TEXT("Grabber reporting for duty: %s."), *ObjectName);

	// Find attached physics handle
	FindPhysicsHandleComponent(ObjectName);

	// Look for attached input componet (only appears at runtime).
	SetupInputComponent(ObjectName);
}


// Find the physics handle component.
void UGrabber::FindPhysicsHandleComponent(FString &ObjectName)
{
	/// look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find the default pawn physics handle for %s."), *ObjectName);

	}
}

// Find the input component for grabbing.
void UGrabber::SetupInputComponent(FString &ObjectName)
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp, Display, TEXT("Found the InputComponent for %s."), *ObjectName);
		// bind the input action
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find the input component for %s."), *ObjectName);

	}
}
void UGrabber::Grab() 
{
	// line trace and try and reach any actors with a collision channel set
	auto ObjectHit = GetFirstPhysicsBodyInReach();
	if (ObjectHit.GetActor())
	{
		APlayerController *PlayerController = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetOwner()->GetWorld());
		FVector PlayerViewPosition;
		FRotator PlayerViewRotation;
		PlayerController->GetPlayerViewPoint(OUT PlayerViewPosition, OUT PlayerViewRotation);
		// We hit something so attach a physics handle
		auto ComponentToGrab = ObjectHit.GetComponent();

		/// attach physics handle
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			PlayerViewRotation
		);
	}
	UE_LOG(LogTemp, Display, TEXT("Grab pressed."))
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Display, TEXT("Release pressed."))
	// Release physics handle
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PhysicsHandle->GrabbedComponent)
	{
		float mass = PhysicsHandle->GrabbedComponent->GetMass();
		//TODO if too heavy make it drag, not lift
		if (mass < 30.f)
		{
			APlayerController *PlayerController = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetOwner()->GetWorld());
			FVector PlayerViewPosition;
			FRotator PlayerViewRotation;
			PlayerController->GetPlayerViewPoint(OUT PlayerViewPosition, OUT PlayerViewRotation);

			FVector LineTraceEnd = GetLineTraceEnd(PlayerViewPosition, PlayerViewRotation);
			UE_LOG(LogTemp, Display, TEXT("Holding item at position: %s |  %s"), *PlayerViewPosition.ToString(), *LineTraceEnd.ToString())

			// move the object we are holding
			PhysicsHandle->SetTargetLocation(LineTraceEnd);
		}
	}
}
FHitResult UGrabber::GetFirstPhysicsBodyInReach() 
{
	// if the physics handle is attached, move the object that we are holding

	APlayerController *PlayerController = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetOwner()->GetWorld());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Null PlayerController."));
		return FHitResult();
	}
	FVector PlayerViewPosition;
	FRotator PlayerViewRotation;
	PlayerController->GetPlayerViewPoint(OUT PlayerViewPosition, OUT PlayerViewRotation);

	/// Log the players position and rotation to test
	UE_LOG(LogTemp, Display, TEXT("First physics body in reach: Player view position is: %s | %s"), *PlayerViewPosition.ToString(), *PlayerViewRotation.ToString())

	// calculate the reach
	FVector RotatorVector = PlayerViewRotation.Vector()*Reach;
	FVector LineTraceEnd = PlayerViewPosition + RotatorVector;

	/// For debugging draw a red trace to visualize
	/// /255,0,0 is red
	/// DrawDebugLine(GetWorld(), PlayerViewPosition, LineTraceEnd, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);

	/// Setup query params. Don't want complex traces to improve performance. Also ignore ourselves
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	/// line trace (ray cast) out to the reach distance
	FHitResult ObjectHit;
	GetWorld()->LineTraceSingleByObjectType(OUT ObjectHit, PlayerViewPosition, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);
	/// See what we hit
	AActor * ActorHit = ObjectHit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("The object hit: %s"), *ActorHit->GetName());
	}
	return ObjectHit;
}

FVector UGrabber::GetPlayerViewPosition()
{

	// if the physics handle is attached, move the object that we are holding
	APlayerController *PlayerController = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetOwner()->GetWorld());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Null PlayerController."));
		return FVector();
	}
	FVector PlayerViewPosition;
	FRotator PlayerViewRotation;
	PlayerController->GetPlayerViewPoint(OUT PlayerViewPosition, OUT PlayerViewRotation);
	return PlayerViewPosition;
}

FVector UGrabber::GetLineTraceEnd(FVector PlayerViewPosition, FRotator PlayerViewRotation)
{
	/// Log the players position and rotation to test
	UE_LOG(LogTemp, Display, TEXT("In GetLineTraceEnd Player view position is: %s | rotation is  %s"), *PlayerViewPosition.ToString(), *PlayerViewRotation.ToString())

	// calculate the reach
	FVector RotatorVector = PlayerViewRotation.Vector()*Reach;
	FVector LineTraceEnd = PlayerViewPosition + RotatorVector;
	return LineTraceEnd;
}



