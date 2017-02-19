// OneStore Plugin
// Created by NextStage(Junsu Park)
// Copyright (c) 2016-2017 NextStage. All Rights Reserved.

#include "OneStorePrivatePCH.h"
#include "OneStoreIapPurchaseCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UCreateSessionCallbackProxy

#if PLATFORM_ANDROID
	void AndroidThunkJava_OneStoreIapRequestPurchase(FString ProductID, FString ProductTitle, FString TID, FString BPInfo);
#endif

UOneStoreIapPurchaseCallbackProxy::UOneStoreIapPurchaseCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UOneStoreIapPurchaseCallbackProxy* UOneStoreIapPurchaseCallbackProxy::OneStoreIapPurchase(UObject* WorldContextObject, const FString& ProductID, const FString& ProductTitle, const FString& TID, const FString& BPInfo)
{
	UOneStoreIapPurchaseCallbackProxy* Proxy = NewObject<UOneStoreIapPurchaseCallbackProxy>();
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->ProductID = ProductID;
	Proxy->ProductTitle = ProductTitle;
	Proxy->TID = TID;
	Proxy->BPInfo = BPInfo;
	return Proxy;
}

void UOneStoreIapPurchaseCallbackProxy::Activate()
{
	UE_LOG(LogOneStore, Warning, TEXT("OneStore Iap Purchase Activate"));

	if (!UOneStoreFunction::IsEnableOneStore())
	{
		UE_LOG(LogOneStore, Warning, TEXT("OneStore plugin is disable."));
		FInAppPurchaseProductInfo Dummy;
		OnFailure.Broadcast(Dummy);
	}

	StaticOneStoreIapPurchaseDelegate.BindUObject(this, &UOneStoreIapPurchaseCallbackProxy::OnCompleteOneStoreIapPurchase);

#if PLATFORM_ANDROID
	AndroidThunkJava_OneStoreIapRequestPurchase(ProductID, ProductTitle, TID, BPInfo);
#endif
}

void UOneStoreIapPurchaseCallbackProxy::OnCompleteOneStoreIapPurchase(bool bSuccess, FInAppPurchaseProductInfo InAppPurchaseProductInfo)
{
	if (bSuccess)
	{
		OnSuccess.Broadcast(InAppPurchaseProductInfo);
	}

	else
	{
		OnFailure.Broadcast(InAppPurchaseProductInfo);
	}
}



#if PLATFORM_ANDROID
void AndroidThunkJava_OneStoreIapRequestPurchase(FString ProductID, FString ProductTitle, FString TID, FString BPInfo)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring JavaProductIdentifier = Env->NewStringUTF(TCHAR_TO_UTF8(*ProductID));
		jstring JavaProductTitle = Env->NewStringUTF(TCHAR_TO_UTF8(*ProductTitle));
		jstring JavaTID = Env->NewStringUTF(TCHAR_TO_UTF8(*TID));
		jstring JavaBPInfo = Env->NewStringUTF(TCHAR_TO_UTF8(*BPInfo));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_OneStoreIapRequestPurchase", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, JavaProductIdentifier, JavaProductTitle, JavaTID, JavaBPInfo);
		Env->DeleteLocalRef(JavaProductIdentifier);
		Env->DeleteLocalRef(JavaProductTitle);
		Env->DeleteLocalRef(JavaTID);
		Env->DeleteLocalRef(JavaBPInfo);
	}
}

extern "C" void Java_com_onestore_OnestoreIapHelper_nativeOnPaymentComplete(JNIEnv* jenv, jobject thiz, jboolean bSuccess, jstring JavaProductID, jstring JavaTxID, jstring JavaReceipt)
{
	UE_LOG(LogOneStore, Warning, TEXT("[UE4] = Call nativeOnPaymentComplete"));

	FInAppPurchaseProductInfo NewProductInfo;

	const char* charsProductId = jenv->GetStringUTFChars(JavaProductID, 0);
	NewProductInfo.Identifier = FString(UTF8_TO_TCHAR(charsProductId));
	jenv->ReleaseStringUTFChars(JavaProductID, charsProductId);
	jenv->DeleteLocalRef(JavaProductID);

	const char* charsTxID = jenv->GetStringUTFChars(JavaTxID, 0);
	NewProductInfo.TransactionIdentifier = FString(UTF8_TO_TCHAR(charsTxID));
	jenv->ReleaseStringUTFChars(JavaTxID, charsTxID);
	jenv->DeleteLocalRef(JavaTxID);

	const char* charsReceipt = jenv->GetStringUTFChars(JavaReceipt, 0);
	NewProductInfo.ReceiptData = FString(UTF8_TO_TCHAR(charsReceipt));
	jenv->ReleaseStringUTFChars(JavaReceipt, charsReceipt);
	jenv->DeleteLocalRef(JavaReceipt);

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\nProduct Identifier: %s, Name: %s, Price: %s\n"),
		*NewProductInfo.Identifier,
		*NewProductInfo.TransactionIdentifier,
		*NewProductInfo.ReceiptData);

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
	{
		UOneStoreIapPurchaseCallbackProxy::StaticOneStoreIapPurchaseDelegate.Execute(bSuccess, NewProductInfo);
		UOneStoreIapPurchaseCallbackProxy::StaticOneStoreIapPurchaseDelegate.Unbind();
	}),
		TStatId(),
		nullptr,
		ENamedThreads::GameThread
		);
}
#endif

UOneStoreIapPurchaseCallbackProxy::FStaticOneStoreIapPurchase UOneStoreIapPurchaseCallbackProxy::StaticOneStoreIapPurchaseDelegate;