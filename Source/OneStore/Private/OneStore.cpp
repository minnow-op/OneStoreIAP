// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#include "OneStorePrivatePCH.h"
#include "OneStoreSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "OneStore"

class FOneStore : public IOneStore
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FOneStore, OneStore)
DEFINE_LOG_CATEGORY(LogOneStore);

void FOneStore::StartupModule()
{
	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "OneStore",
			LOCTEXT("RuntimeSettingsName", "OneStore Settings"),
			LOCTEXT("RuntimeSettingsDescription", "OneStore Settings"),
			GetMutableDefault<UOneStoreSettings>()
		);
	}
}

void FOneStore::ShutdownModule()
{

}
