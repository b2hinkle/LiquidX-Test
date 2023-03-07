// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LiquidX_Test : ModuleRules
{
	public LiquidX_Test(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
