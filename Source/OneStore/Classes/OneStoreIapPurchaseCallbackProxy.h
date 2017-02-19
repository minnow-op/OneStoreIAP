// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.
#pragma once

#include "OnlineBlueprintCallProxyBase.h"
#include "OnlineSubsystem.h"
#include "OnlineStoreInterface.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#include "OneStoreIapPurchaseCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneStoreIapPurchaseResult, const FInAppPurchaseProductInfo&, InAppPurchaseProductInfo);

UCLASS(MinimalAPI)
class UOneStoreIapPurchaseCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
	FOneStoreIapPurchaseResult OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
	FOneStoreIapPurchaseResult OnFailure;

	// Creates a session with the default online subsystem
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "OneStore")
	static UOneStoreIapPurchaseCallbackProxy* OneStoreIapPurchase(UObject* WorldContextObject, const FString& ProductID, const FString& ProductTitle, const FString& TID, const FString& BPInfo);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

public:
	// ¿ø½ºÅä¾î ÀÎ¾Û ÇÁ·Î´öÆ® ID ¿äÃ» ½ºÅÂÆ½ µ¨¸®°ÔÀÌÆ®
	DECLARE_DELEGATE_TwoParams(FStaticOneStoreIapPurchase, bool, FInAppPurchaseProductInfo);
	static FStaticOneStoreIapPurchase StaticOneStoreIapPurchaseDelegate;

private:

	FString ProductID;
	FString ProductTitle;
	FString TID;
	FString BPInfo;

	void OnCompleteOneStoreIapPurchase(bool bSuccess, FInAppPurchaseProductInfo InAppPurchaseProductInfo);

	// The world context object in which this call is taking place
	UObject* WorldContextObject;

};
