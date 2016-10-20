// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// This module must be loaded "PreLoadingScreen" in the .uproject file, otherwise it will not hook in time!

public class TDLoadingScreen : ModuleRules
{
    public TDLoadingScreen(TargetInfo Target)
	{
		//PrivateIncludePaths.Add("../../TopDownShooter/Source/TDLoadingScreen/Private");

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"MoviePlayer",
				"Slate",
				"SlateCore",
				"InputCore"
			}
		);
	}
}
