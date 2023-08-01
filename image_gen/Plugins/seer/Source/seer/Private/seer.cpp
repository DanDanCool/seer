// Copyright Epic Games, Inc. All Rights Reserved.

#include "seer.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "VisionThread.h"

#define LOCTEXT_NAMESPACE "FseerModule"

void FseerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("seer")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/Win64"));

	TArray<FString> libraries = {
		TEXT("seer_lib.dll"),
	};

	for (FString& library : libraries) {
		FString path = FPaths::Combine(*LibraryPath, library);
		void* handle = !path.IsEmpty() ? FPlatformProcess::GetDllHandle(*path) : nullptr;
		if (!handle) {
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
		}

		LibraryHandle.Add(handle);
	}

	int ok = SEERTestLoadFunction(7);
	check(ok);
	if (!ok) {
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FseerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	for (auto& handle : LibraryHandle) {
		FPlatformProcess::FreeDllHandle(handle);
		handle = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FseerModule, seer)
