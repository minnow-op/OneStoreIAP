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
