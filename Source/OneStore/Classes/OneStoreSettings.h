// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#pragma once

#include "OneStoreSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UOneStoreSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "OneStore", meta = (DisplayName = "EnableOneStore"))
	bool bEnableOneStore;

	UPROPERTY(Config, EditAnywhere, Category = "OneStore", meta = (DisplayName = "Debug Mode"))
	bool bDebugMode;

	UPROPERTY(Config, EditAnywhere, Category = "OneStore", meta = (DisplayName = "ApplicationID"))
	FString ApplicationID;
};
