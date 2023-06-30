// Fill out your copyright notice in the Description page of Project Settings.


#include "capture.h"
#include "CoreFwd.h"

// Sets default values for this component's properties
Ucapture::Ucapture()
	: Super()
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

	if (!resource->ReadPixels(pixels)) {
		UE_LOG(LogTemp, Display, TEXT("reading pixels failed"));
	}

	if (!CapturedActor) return;

	FMinimalViewInfo viewinfo;
	GetCameraView(DeltaTime, viewinfo);

	FMatrix proj = viewinfo.CalculateProjectionMatrix();

	FMatrix viewrotation = FInverseRotationMatrix(viewinfo.Rotation) * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));
	FMatrix viewproj = FTranslationMatrix(-viewinfo.Location) * viewrotation * proj;

	FVector position, extents;
	CapturedActor->GetActorBounds(false, position, extents, true);

	TArray<FVector> vertices = { FVector(1, 1, 1), FVector(1, 1, -1), FVector(1, -1, -1), FVector(1, -1, 1), FVector(-1, 1, 1), FVector(-1, 1, -1), FVector(-1, -1, -1), FVector(-1, -1, 1) };
	TArray<double> xvals, yvals;
	for (auto& v : vertices) {
		FVector vertex = position + (extents * v);
		FVector4 screenspace = viewproj.TransformFVector4(FVector4(vertex, 1.0));
		FVector4 ndc = screenspace / screenspace.W;
		xvals.Add(ndc.X);
		yvals.Add(ndc.Y);
	}

	xvals.Sort();
	yvals.Sort();

	FVector4f bbox;
	bbox.X = xvals[0];
	bbox.Y = yvals[0];
	bbox.Z = xvals[7];
	bbox.W = yvals[7];

	boxes.Empty();
	boxes.Add(bbox);

	if (!_Proxy.IsRunning()) {
		_Proxy.Run(pixels, boxes, TextureTarget->SizeX, TextureTarget->SizeY);
	}
	else {
		_Proxy.NextFrame(pixels, boxes);
	}
}

