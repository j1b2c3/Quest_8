// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Quest_8 : ModuleRules
{
	public Quest_8(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput",
			"UMG"
		});
	}
}