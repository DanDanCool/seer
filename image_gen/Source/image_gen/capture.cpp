// Fill out your copyright notice in the Description page of Project Settings.


#include "capture.h"

// Sets default values for this component's properties
Ucapture::Ucapture()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void Ucapture::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void Ucapture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	FRenderTarget* resource = TextureTarget->GameThread_GetRenderTargetResource();
	
	TArray<FColor>& buffer = _Proxy.GetBuffer();
	if (!resource->ReadPixels(buffer)) {
		UE_LOG(LogTemp, Display, TEXT("reading pixels failed"));
	}

	if (!_Proxy.IsRunning()) {
		_Proxy.Run(buffer, TextureTarget->SizeX, TextureTarget->SizeY);
	}
	else {
		_Proxy.NextFrame(buffer);
	}
}

