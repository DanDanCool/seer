// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "control.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IMAGE_GEN_API Ucontrol : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled;

	UPROPERTY(EditAnywhere)
	float ImpulseMultiplier;

	UPROPERTY(EditAnywhere)
		FVector Direction;

public:	
	// Sets default values for this component's properties
	Ucontrol();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
