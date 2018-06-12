// Copyright Josh Cleland 2018

#include "Grabber.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();

	SetupInputComponent();

}

/// Find PhysicsHandle and InputComponent from pawn
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	//Check if Physics handle exists
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics handle on %s"), *GetOwner()->GetName());
	}
}

/// Set up inputs based on input component from FindPhysicsHandleComponent
void UGrabber::SetupInputComponent()
{
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Input Component on %s"), *GetOwner()->GetName());
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// Draw line from player to line trace end, assign Hit to the first physics body hit
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	if (HitResult.IsValidBlockingHit()) {
		UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *HitResult.GetActor()->GetName());
	}
	return HitResult;
}


/// Sets out parameters as Location and Rotation from playerController
 const FVector UGrabber::GetLineTraceEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

 const FVector UGrabber::GetLineTraceStart()
 {
	 FVector PlayerViewPointLocation;
	 FRotator PlayerViewPointRotation;

	 GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		 OUT PlayerViewPointLocation,
		 OUT PlayerViewPointRotation
	 );

	 return PlayerViewPointLocation;
 }

/// Perform ray trace and attach physics handle to Hit result
void UGrabber::Grab() {
	// Try and reach actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent(); //Gets static mesh i.e chair
	auto ActorHit = HitResult.GetActor();

	// If we hit something attach
	if (ActorHit) {
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None, //No bones needed
			ComponentToGrab->GetOwner()->GetActorLocation()
		);
	}	
}

/// Release physics handle
void UGrabber::Release() {
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If physics handle is attached - move
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

