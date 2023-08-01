// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class seer : ModuleRules
{
	public seer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		string PROJECT_ROOT = Path.GetFullPath(Path.Combine(PluginDirectory, "../../.."));
        string include = Path.Combine(PROJECT_ROOT, "src");
        Console.Out.WriteLine(include);
        PublicIncludePaths.Add(include);

		string THIRDPARTY_FOLDER = "$(PluginDir)/Binaries/ThirdParty/Win64";
		Console.Out.WriteLine(THIRDPARTY_FOLDER);

        PublicAdditionalLibraries.Add(Path.Combine(THIRDPARTY_FOLDER, "seer_lib.lib"));
		RuntimeDependencies.Add(Path.Combine(THIRDPARTY_FOLDER, "seer_lib.dll"));

        PublicDelayLoadDLLs.AddRange(new string[]
        {
            "seer_lib.dll",
        });

        PrivateDefinitions.Add("EXPORT");

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
				"Projects",
                "Engine",
                "InputCore",
				"UnrealEd"
				// ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
