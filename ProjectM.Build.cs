// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectM : ModuleRules
{
	public ProjectM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("ProjectM");
        PrivatePCHHeaderFile = "ProjectM.h";

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",

			"InputCore",
			"EnhancedInput",

            "GameFeatures",
            "ModularGameplay",
            "ModularGameplayActors",

			"CommonUser",
			"CommonGame",
			"CommonUI",
			"UMG",
			"UIExtension",

            "Slate",
            "SlateCore",

            "GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",

			"NetCore",           // 네트워크 관련
// 			"Serialization",     // FFastArraySerializer 관련
// 			"GuidUtilities"      // FGuidReferences 관련

			"AIModule",

			"NavigationSystem",
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
