// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#pragma once

#include "ModuleManager.h"


class IOneStore : public IModuleInterface
{

public:

	static inline IOneStore& Get()
	{
		return FModuleManager::LoadModuleChecked< IOneStore >("OneStore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OneStore");
	}
};
