// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!Owner) {
		UE_LOG(LogTemp, Error, TEXT("Open door component is apparently missing an owner"));
	}
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetTotalMassOnplate() > TriggerMass)
	{
		OnOpen.Broadcast();
	}
	else {
		OnClose.Broadcast();
	}

}

float UOpenDoor::GetTotalMassOnplate()
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActorsArray;
	//Find all the overlappinpg actors
	if (!PressurePlate) { return 0.f; } ///bailer
	PressurePlate->GetOverlappingActors(OUT OverlappingActorsArray);
	//iterate and add
	for (const auto* actor : OverlappingActorsArray)
	{
		TotalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s on plate"), *actor->GetName())
	}

	return TotalMass;
}

