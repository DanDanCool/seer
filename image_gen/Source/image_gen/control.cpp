// Fill out your copyright notice in the Description page of Project Settings.

#include "control.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
Ucontrol::Ucontrol()
	: bEnabled(false), ImpulseMultiplier(1.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	UE_LOG(LogTemp, Display, TEXT("created control component"));
	// ...
}


// Called when the game starts
void Ucontrol::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void Ucontrol::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bEnabled) return;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* actor = GetOwner();
	TArray<UStaticMeshComponent*> bodies;
	actor->GetComponents<UStaticMeshComponent>(bodies);

	FVector impulse = Direction.GetUnsafeNormal() * ImpulseMultiplier;
	int count = 0;
	for (auto* body : bodies) {
		//UE_LOG(LogTemp, Display, TEXT("added impulse to body %d"), count);
		body->AddImpulse(impulse);
		count++;
	}
}

