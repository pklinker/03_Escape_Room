// Copyright Paul Klinker 2018

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetupInputComponent(FString &ObjectName);

	void FindPhysicsHandleComponent(FString &ObjectName);

	FHitResult GetFirstPhysicsBodyInReach();

	FVector GetPlayerViewPosition();
	FVector GetLineTraceEnd(FVector, FRotator);
public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	float Reach = 120.0f; // reach of player in centimeters
	UPhysicsHandleComponent *PhysicsHandle = nullptr;
	UInputComponent *InputComponent = nullptr;
	// Ray cast and grab what is in reach
	void Grab();
	// Release the object being held.
	void Release();
};
