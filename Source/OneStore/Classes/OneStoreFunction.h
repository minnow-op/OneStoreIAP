// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#pragma once
#include "OneStoreFunction.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ONESTORE_API UOneStoreFunction : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	// ������� �ξ� ��� ����
	UFUNCTION(BlueprintPure, Category = "OneStore")
	static bool IsEnableOneStore();

	UFUNCTION(BlueprintPure, Category = "OneStore")
	static FString GetOneStoreApplicationID();

};
