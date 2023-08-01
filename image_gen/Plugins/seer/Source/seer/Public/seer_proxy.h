#pragma once

#include "CoreMinimal.h"
#include "ResourcePool.h"

#ifdef EXPORT
#define EXP_API __declspec(dllexport)
#else
#define EXP_API __declspec(dllimport)
#endif

class VisionThread;

class EXP_API SeerProxy {
public:
	SeerProxy();
	~SeerProxy();

	void Run(TArray<FColor>& pixels, TArray<FVector4f>& boxes, int x, int y);
	void NextFrame(TArray<FColor>& pixels, TArray<FVector4f>& boxes);

	bool IsRunning();

private:
	using BufType = TArray<FColor>;

	VisionThread* _Thread;
	bool _Run;
};
