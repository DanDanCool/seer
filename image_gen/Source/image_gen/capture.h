// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "seer_proxy.h"

#include "capture.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IMAGE_GEN_API Ucapture : public USceneCaptureComponent2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> CapturedActor;

	SeerProxy _Proxy;

	TArray<FColor> pixels;
	TArray<FVector4f> boxes;

	bool bRecord;

public:
	// Sets default values for this component's properties
	Ucapture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
