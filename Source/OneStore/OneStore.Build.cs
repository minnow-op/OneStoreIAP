// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OneStore : ModuleRules
	{
        public OneStore(TargetInfo Target)
        {
            PublicIncludePaths.AddRange(
                new string[]
                {
				    // PublicIncludePath
			    }
                );

            PrivateIncludePaths.AddRange(
                new string[]
                {
                    // PrivateIncludePath
                    "OneStore/Private"
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    // PublicDependencyModule
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "OnlineSubsystem",
                    "OnlineSubsystemUtils"
			    }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    // PrivateDependencyModule
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
				    // DynamicallyLoadedModule
			    }
                );

            PublicIncludePathModuleNames.AddRange(
                new string[]
                {
                    "Launch"
                }
                );

            PrivateIncludePathModuleNames.AddRange(
                new string[]
                {
                    // PrivateIncludePathModule
					"Settings"
                }
                );


            if (Target.Platform == UnrealTargetPlatform.Android)
            {
                string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, BuildConfiguration.RelativeEnginePath);
                AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "OneStore_APL.xml")));
            }
        }
	}
}
