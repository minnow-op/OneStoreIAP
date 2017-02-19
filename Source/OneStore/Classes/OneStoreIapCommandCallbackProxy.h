// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.
#pragma once

#include "OnlineBlueprintCallProxyBase.h"
#include "OnlineSubsystem.h"
#include "OnlineStoreInterface.h"
#include "Kismet/KismetMathLibrary.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#include "OneStoreIapCommandCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneStoreIapCommandResult, const TArray<FInAppPurchaseProductInfo>&, InAppPurchaseProductInfoList);

UCLASS(MinimalAPI)
class UOneStoreIapCommandCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
	FOneStoreIapCommandResult OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
	FOneStoreIapCommandResult OnFailure;

	// Creates a session with the default online subsystem
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "OneStore")
	static UOneStoreIapCommandCallbackProxy* OneStoreIapCommand(UObject* WorldContextObject);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

public:
	// ¿ø½ºÅä¾î ÀÎ¾Û ÇÁ·Î´öÆ® ID ¿äÃ» ½ºÅÂÆ½ µ¨¸®°ÔÀÌÆ®
	DECLARE_DELEGATE_TwoParams(FStaticOneStoreIapCommand, bool, const TArray<FInAppPurchaseProductInfo>&);
	static FStaticOneStoreIapCommand StaticOneStoreIapCommandDelegate;

private:
	void OnCompleteOneStoreIapCommand(bool bSuccess, const TArray<FInAppPurchaseProductInfo>& InAppPurchaseProductInfoList);

	// The world context object in which this call is taking place
	UObject* WorldContextObject;

};
