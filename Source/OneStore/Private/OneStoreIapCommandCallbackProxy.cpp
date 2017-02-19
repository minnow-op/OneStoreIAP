// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#include "OneStorePrivatePCH.h"
#include "OneStoreIapCommandCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UCreateSessionCallbackProxy

#if PLATFORM_ANDROID
	void AndroidThunkJava_OneStoreIapRequestCommand();
#endif

UOneStoreIapCommandCallbackProxy::UOneStoreIapCommandCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UOneStoreIapCommandCallbackProxy* UOneStoreIapCommandCallbackProxy::OneStoreIapCommand(UObject* WorldContextObject)
{
	UOneStoreIapCommandCallbackProxy* Proxy = NewObject<UOneStoreIapCommandCallbackProxy>();
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UOneStoreIapCommandCallbackProxy::Activate()
{
	UE_LOG(LogOneStore, Warning, TEXT("OneStore Iap Command Activate"));

	if (!UOneStoreFunction::IsEnableOneStore())
	{
		UE_LOG(LogOneStore, Warning, TEXT("OneStore plugin is disable."));
		TArray<FInAppPurchaseProductInfo> Dummy;
		OnFailure.Broadcast(Dummy);
	}

	StaticOneStoreIapCommandDelegate.BindUObject(this, &UOneStoreIapCommandCallbackProxy::OnCompleteOneStoreIapCommand);

#if PLATFORM_ANDROID
	AndroidThunkJava_OneStoreIapRequestCommand();
#endif
}

void UOneStoreIapCommandCallbackProxy::OnCompleteOneStoreIapCommand(bool bSuccess, const TArray<FInAppPurchaseProductInfo>& InAppPurchaseProductInfoList)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast(InAppPurchaseProductInfoList);
	}

	else
	{
		OnFailure.Broadcast(InAppPurchaseProductInfoList);
	}
}


#if PLATFORM_ANDROID
void AndroidThunkJava_OneStoreIapRequestCommand()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_OneStoreIapRequestCommand", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

extern "C" void Java_com_onestore_OnestoreIapHelper_nativeOnCommandComplete(JNIEnv* jenv, jobject thiz, jboolean bSuccess, jobjectArray productIDs, jobjectArray titles, jobjectArray prices)
{
	UE_LOG(LogOneStore, Warning, TEXT("[UE4] = Call nativeOnCommandComplete"));

	TArray<FInAppPurchaseProductInfo> ProvidedProductInformation;

	if (jenv && bSuccess)
	{
		jsize NumProducts = jenv->GetArrayLength(productIDs);
		jsize NumTitles = jenv->GetArrayLength(titles);
		jsize NumPrices = jenv->GetArrayLength(prices);

		ensure((NumProducts == NumTitles) && (NumProducts == NumPrices));

		for (jsize Idx = 0; Idx < NumProducts; Idx++)
		{
			// Build the product information strings.

			FInAppPurchaseProductInfo NewProductInfo;

			jstring NextId = (jstring)jenv->GetObjectArrayElement(productIDs, Idx);
			const char* charsId = jenv->GetStringUTFChars(NextId, 0);
			NewProductInfo.Identifier = FString(UTF8_TO_TCHAR(charsId));
			jenv->ReleaseStringUTFChars(NextId, charsId);
			jenv->DeleteLocalRef(NextId);

			jstring NextTitle = (jstring)jenv->GetObjectArrayElement(titles, Idx);
			const char* charsTitle = jenv->GetStringUTFChars(NextTitle, 0);
			NewProductInfo.DisplayName = FString(UTF8_TO_TCHAR(charsTitle));
			jenv->ReleaseStringUTFChars(NextTitle, charsTitle);
			jenv->DeleteLocalRef(NextTitle);

			jstring NextPrice = (jstring)jenv->GetObjectArrayElement(prices, Idx);
			const char* charsPrice = jenv->GetStringUTFChars(NextPrice, 0);
			FString DisplayPrice = FString(UTF8_TO_TCHAR(charsPrice));
			NewProductInfo.RawPrice = FCString::Atof(*DisplayPrice);
			int32 IntPrice = (int32)NewProductInfo.RawPrice;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(NewProductInfo.RawPrice, float(IntPrice), 0.001f))
			{
				NewProductInfo.DisplayPrice = FString::FromInt(IntPrice);
			}

			else
			{
				NewProductInfo.DisplayPrice = DisplayPrice;
			}

			//NewProductInfo.DisplayPrice = FString(UTF8_TO_TCHAR(charsPrice));
			jenv->ReleaseStringUTFChars(NextPrice, charsPrice);
			jenv->DeleteLocalRef(NextPrice);

			ProvidedProductInformation.Add(NewProductInfo);

			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\nProduct Identifier: %s, Name: %s, Price: %s\n"),
				*NewProductInfo.Identifier,
				*NewProductInfo.DisplayName,
				*NewProductInfo.DisplayPrice);
		}
	}

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
	{
		UOneStoreIapCommandCallbackProxy::StaticOneStoreIapCommandDelegate.Execute(bSuccess, ProvidedProductInformation);
		UOneStoreIapCommandCallbackProxy::StaticOneStoreIapCommandDelegate.Unbind();
	}),
		TStatId(),
		nullptr,
		ENamedThreads::GameThread
		);
}


#endif
UOneStoreIapCommandCallbackProxy::FStaticOneStoreIapCommand UOneStoreIapCommandCallbackProxy::StaticOneStoreIapCommandDelegate;