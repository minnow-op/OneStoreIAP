// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#include "OneStorePrivatePCH.h"

UOneStoreFunction::UOneStoreFunction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UOneStoreFunction::IsEnableOneStore()
{
	if (UGameplayStatics::GetPlatformName() == TEXT("Android"))
	{
		bool bEnableOneStore = false;
		GConfig->GetBool(TEXT("/Script/OneStore.OneStoreSettings"), TEXT("bEnableOneStore"), bEnableOneStore, GEngineIni);
		return bEnableOneStore;
	}

	return false;
}

FString UOneStoreFunction::GetOneStoreApplicationID()
{
	if (UGameplayStatics::GetPlatformName() == TEXT("Android"))
	{
		FString ApplicationID = FString();
		GConfig->GetString(TEXT("/Script/OneStore.OneStoreSettings"), TEXT("ApplicationID"), ApplicationID, GEngineIni);
		return ApplicationID;
	}

	return FString();
}
