// Fill out your copyright notice in the Description page of Project Settings.


#include "capture.h"
#include "CoreFwd.h"
#include "Editor.h"

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
	bRecord = GEditor->IsPlaySessionInProgress();
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

	if (!CapturedActor || !bRecord) return;

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

	boxes.Empty();
	/*
	for (int i = 0; i < 4; i += 2) {
		FVector4f bbox;
		bbox.X = xvals[0 + i];
		bbox.Y = yvals[0 + i];
		bbox.Z = xvals[7 - i];
		bbox.W = yvals[7 - i];
		boxes.Add(bbox);
	} */

	FVector4f avg;
	avg.X = (xvals[0] + xvals[2]) / 2;
	avg.Y = (yvals[0] + yvals[2]) / 2;
	avg.Z = (xvals[7] + xvals[5]) / 2;
	avg.W = (yvals[7] + yvals[5]) / 2;
	boxes.Add(avg);


	if (!_Proxy.IsRunning()) {
		_Proxy.Run(pixels, boxes, TextureTarget->SizeX, TextureTarget->SizeY);
	}
	else {
		_Proxy.NextFrame(pixels, boxes);
	}
}

