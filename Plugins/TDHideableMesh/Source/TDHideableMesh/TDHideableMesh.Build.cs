// Copyright 2016 (c) V.Khmelevskiy <absxv@yandex.ru>

using UnrealBuildTool;

public class TDHideableMesh : ModuleRules
{
    public TDHideableMesh(TargetInfo Target)
	{

        PublicIncludePaths.Add("TDHideableMesh/Public");

		PrivateIncludePaths.Add("TDHideableMesh/Private");

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				 "Engine",
			}
		);
	}
}
