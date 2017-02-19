package com.onestore;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import com.onestore.client.exception.InvalidMetadataException;
import com.skplanet.dodo.IapPlugin;
import com.skplanet.dodo.IapResponse;
import com.skplanet.dodo.ProcessType;
import com.skplanet.dodo.ReceiptVerificationTask;
import com.skplanet.dodo.pdu.Action;

import com.skplanet.dodo.helper.ConverterFactory;
import com.skplanet.dodo.pdu.Response;
import java.util.ArrayList;

/**
 *  Onestore IAP(OIAP) SDK와 연동하는 클래스
 *
 *  주요 역할 : OIAP SDK API 호출 후, json 형태의 결과를 String 형태로 native 메소드를 통해 전달
 */
public class OnestoreIapHelper {
    private static final String LOG_TAG = OnestoreIapHelper.class.getSimpleName();
    private final Activity mActivity;
    private final boolean mIsDebugMode;
	private final String ApplicationID;
    private IapPlugin mIapPlugin;

    /**
     *  생성자
     *
     *  @param gameActivity  activity 인스턴스, 결제 화면 랜딩을 위해 필요
     *  @param debugMode     PlugIn의 debugMode 세팅 인자
     *
     *  @throws InvalidMetadataException SDK 버전이 4보다 낮을 경우,
     *  @throws IllegalArgumentException gameActivity 파람 값이 null 일 경우 발생되는 예외
     */
    public OnestoreIapHelper(Activity gameActivity, boolean debugMode, String ApplicationID) throws InvalidMetadataException, IllegalArgumentException {
        if(gameActivity == null){
            throw new IllegalArgumentException("activity param is null");
        }

        this.mActivity = gameActivity;
        this.mIsDebugMode = debugMode;
		this.ApplicationID = ApplicationID;
        iapInit();
    }

    /**
     *  OIAP SDK 초기화
     *
     *  생성자에서 세팅한 모드(릴리즈/디버그)로 초기화
     */
    private void iapInit(){
        try{
            if (mIsDebugMode) {
                mIapPlugin = IapPlugin.getPlugin(mActivity, IapPlugin.DEVELOPMENT_MODE);
            } else {
                mIapPlugin = IapPlugin.getPlugin(mActivity, IapPlugin.RELEASE_MODE);
            }
        }catch(InvalidMetadataException e){
            throw new InvalidMetadataException("metadata is invalid. check iap:api_version value. we support iap:api_version 4 +");
        }
    }

    /**
     *  결제 요청 : 결제 화면으로 랜딩
     *
     *  @param  appId       : Application ID, 통합 개발자센터에 Application을 등록하면 그 Application에 발급되는 고유한 ID
     *  @param  pId         : Product ID, 등록한 Application에서 판매할 In-App상품을 추가하면 발급되는 고유한 In-App상품 ID
     *  @param  pName       : Product Name, 결제 화면에서 보여지는 상품명
     *  @param  tId         : TID 는 IAP API 를 이용해 생성하지 않고 App 내에서 자체적으로 생성 해야 하며 TID 조회 API 를 사용할 경우, Uniqueness 를 보장해야 함
     *  @param  bpInfo      : 캠페인 통계 등을 위해서 개발사가 자유롭게 사용하는 태그
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     */
    public void requestPurchase(String ProduceID, String ProductTitle, String TID, String BPInfo) {
        mIapPlugin.sendPaymentRequest(ApplicationID, ProduceID, ProductTitle, TID, BPInfo, mRequestPurchaseCallback);
    }

    /**
     *  구매내역 조회 : 아래 사전 조건이 성립되지 않을 경우 관련 UI가 보여질 수 있음
     *      사전조건 1. 원스토어 로그인이 되어 있어야 함
     *      사전조건 2. 필요한 permission이 granted 되어 있어야 함
     *      사전조건 3. 로그인 연동 모듈(OneStore Service) 버전이 최소지원버전 이상이여야 함
     *
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId가 null 값일 경우 발생
     */
    public String doCmdPurchaseHistory(String appId, String ... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandPurchaseHistory(mCmdCallback, ProcessType.FOREGROUND_IF_NEEDED, appId, pIds);
    }

    /**
     *  구매내역 조회 : 아래 사전 조건이 성립되지 않을 경우, onError 콜백으로 리턴 (UI노출 없이 Background 처리를 위한 규격)
     *
     *      사전조건 1. 원스토어 로그인이 되어 있어야 함
     *      사전조건 2. 필요한 permission이 granted 되어 있어야 함
     *      사전조건 3. 로그인 연동 모듈(OneStore Service) 버전이 최소지원버전 이상이여야 함
     *
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId가 null 값일 경우 발생
     */
    public String doCmdPurchaseHistory_bg(String appId, String ... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandPurchaseHistory(mCmdCallback, ProcessType.BACKGROUND_ONLY, appId, pIds);
    }

    /**
     *  상품정보 가져오기 : 상기 사전 조건이 성립되지 않을 경우 관련 UI가 보여질 수 있음
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId가 null 값일 경우 발생
     */
    public String doCmdProductInfo() throws IllegalArgumentException {
        return mIapPlugin.sendCommandProductInfo(mCmdCallback, ProcessType.FOREGROUND_IF_NEEDED, ApplicationID);
    }

    /**
     *  상품정보 가져오기 : 상기 사전 조건이 성립되지 않을 경우, onError 콜백으로 리턴 (UI노출 없이 Background 처리를 위한 규격)
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId가 null 값일 경우 발생
     */
    public String doCmdProductInfo_bg() throws IllegalArgumentException {
        return mIapPlugin.sendCommandProductInfo(mCmdCallback, ProcessType.BACKGROUND_ONLY, ApplicationID);
    }

    /**
     *  구매 가능여부 체크 : 상기 사전 조건이 성립되지 않을 경우 관련 UI가 보여질 수 있음
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId가 null 값일 경우 발생
     */
    public String doCmdCheckPurchasability(String appId, String ... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandCheckPurchasability(mCmdCallback, ProcessType.FOREGROUND_IF_NEEDED, appId, pIds);
    }

    /**
     *  구매 가능여부 체크 : 상기 사전 조건이 성립되지 않을 경우, onError 콜백으로 리턴 (UI노출 없이 Background 처리를 위한 규격)
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId가 null 값일 경우 발생
     */
    public String doCmdCheckPurchasability_bg(String appId, String ... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandCheckPurchasability(mCmdCallback, ProcessType.BACKGROUND_ONLY, appId, pIds);
    }

    /**
     *  구매한 상품 상태 변경 요청(월 정액 해지 요청) : 상기 사전 조건이 성립되지 않을 경우 관련 UI가 보여질 수 있음
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId 혹은 Action 정보가 null 값일 경우 발생
     */
    public String doCmdChangeProduct_withDrawSubscription(String appId, String... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandChangeProductProperties(mCmdCallback, ProcessType.FOREGROUND_IF_NEEDED, appId, Action.cancel_subscription.action(), pIds);
    }

    /**
     *  구매한 상품 상태 변경 요청(월 정액 해지 요청) : 상기 사전 조건이 성립되지 않을 경우, onError 콜백으로 리턴 (UI노출 없이 Background 처리를 위한 규격)
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId 혹은 Action 정보가 null 값일 경우 발생
     */
    public String doCmdChangeProduct_withDrawSubscription_bg(String appId, String... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandChangeProductProperties(mCmdCallback, ProcessType.BACKGROUND_ONLY, appId, Action.cancel_subscription.action(), pIds);
    }

    /**
     *  구매한 상품 상태 변경 요청(상품 포인트 차감 요청) : 상기 사전 조건이 성립되지 않을 경우 관련 UI가 보여질 수 있음
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId 혹은 Action 정보가 null 값일 경우 발생
     */
    public String doCmdChangeProduct_descentPoints(String appId, String... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandChangeProductProperties(mCmdCallback, ProcessType.FOREGROUND_IF_NEEDED, appId, Action.subtract_points.action(), pIds);
    }

    /**
     *  구매한 상품 상태 변경 요청(상품 포인트 차감 요청) : 상기 사전 조건이 성립되지 않을 경우, onError 콜백으로 리턴 (UI노출 없이 Background 처리를 위한 규격)
     *  @return request ID  : 요청 식별 값, 요청 시 OIAP에서 생성해서 발급해주는 값으로써 콜백 응답에서 확인할 수 있음
     *  @throws IllegalArgumentException : appId 혹은 productId 혹은 Action 정보가 null 값일 경우 발생
     */
    public String doCmdChangeProduct_descentPoints_bg(String appId, String... pIds) throws IllegalArgumentException {
        return mIapPlugin.sendCommandChangeProductProperties(mCmdCallback, ProcessType.BACKGROUND_ONLY, appId, Action.subtract_points.action(), pIds);
    }

    /**
     *  전자영수증 검증 요청
     *  @param txId             : 결제 결과 시 전달 받은 값
     *  @param receiptSignData  : 결제 결과 시 전달 받은 값, 실제 영수증 Data
     */
    public void doReceiptVerification(String appId, String txId, String receiptSignData) throws IllegalArgumentException {
        mIapPlugin.sendReceiptVerificationRequest(appId, txId, receiptSignData, mReceiptCallback);
    }

    /**
     *  상기 요청들에 대한 응답 콜백으로써 아래와 같이 분류됨
     *      - 결제화면에 대한 콜백
     *      - 상태정보요청에 대한 콜백
     *      - 전자영수증에 대한 콜백
     *
     *  콜백 응답에 따라 native method(개발사 구현)를 호출
     */
    private IapPlugin.RequestCallback mRequestPurchaseCallback = new IapPlugin.RequestCallback() {
        @Override
        public void onResponse(IapResponse iapResponse) {
            Log.d(LOG_TAG, "onResponse : " + iapResponse.getContentToString());

			Response ResponseInformation = ConverterFactory.getConverter().fromJson(iapResponse.getContentToString());

			if (ResponseInformation == null)
            {
				Log.e(LOG_TAG, "onResponse : ResponseInformation is null");
                nativeOnPaymentComplete(false, "", "", "");
            }

            if (ResponseInformation.result.code.equals("0000"))
            {
				Log.e(LOG_TAG, "onResponse : Success");
                nativeOnPaymentComplete(true, ResponseInformation.result.product.get(0).id, ResponseInformation.result.txid, ResponseInformation.result.receipt);
            }

            else
            {
				Log.e(LOG_TAG, "onResponse : Unknow Error - " + iapResponse.getContentToString());
                nativeOnPaymentComplete(false, "", "", "");
            }
        }

        @Override
        public void onError(String reqId, String errCode, String errMsg) {
            Log.d(LOG_TAG, "onError. reqId : " + reqId + ", errCode : " + errCode + ", errMsg : " + errMsg);

			nativeOnPaymentComplete(false, "", "", "");
        }
    };

    private IapPlugin.RequestCallback mCmdCallback = new IapPlugin.RequestCallback() {
        @Override
        public void onResponse(IapResponse iapResponse) {
            Log.d(LOG_TAG, "onResponse : " + iapResponse.getContentToString());

			ArrayList<String> TitleArrayList = new ArrayList<String>();
			ArrayList<String> PriceArrayList = new ArrayList<String>();
			ArrayList<String> ProductIdArrayList = new ArrayList<String>();

            Response ResponseInformation = ConverterFactory.getConverter().fromJson(iapResponse.getContentToString());

            if (ResponseInformation == null)
            {
                Log.e(LOG_TAG, "[OneStoreManager] - mRequestCommandCallback::onResponse : invalid response data");
                nativeOnCommandComplete(false, ProductIdArrayList.toArray(new String[ProductIdArrayList.size()]), null, null);
            }

            if (ResponseInformation.result.code.equals("0000"))
            {
                for (int i = 0; i < ResponseInformation.result.product.size(); i++)
                {
                    ProductIdArrayList.add(ResponseInformation.result.product.get(i).id);
                    TitleArrayList.add(ResponseInformation.result.product.get(i).name);
                    PriceArrayList.add(String.valueOf(ResponseInformation.result.product.get(i).price));
                }

                Log.i(LOG_TAG, "[OneStoreManager] - mRequestCommandCallback::onResponse : command response succeeded");
                nativeOnCommandComplete(true, ProductIdArrayList.toArray(new String[ProductIdArrayList.size()]), TitleArrayList.toArray(new String[TitleArrayList.size()]), PriceArrayList.toArray(new String[PriceArrayList.size()]));
            }

            else
            {
                Log.e(LOG_TAG, "[OneStoreManager] - mRequestCommandCallback::onResponse : Execpted error");
                nativeOnCommandComplete(false, ProductIdArrayList.toArray(new String[ProductIdArrayList.size()]), null, null);
            }
        }

        @Override
        public void onError(String reqId, String errCode, String errMsg) {
            Log.d(LOG_TAG, "onError. reqId : " + reqId + ", errCode : " + errCode + ", errMsg : " + errMsg);

            nativeOnCommandComplete(false, null, null, null);
        }
    };

    private ReceiptVerificationTask.RequestCallback mReceiptCallback = new ReceiptVerificationTask.RequestCallback() {
        @Override
        public void onResponse(String result) {
            Log.d(LOG_TAG, "onResponse : " + result);
            //nativeOnVerifyReceiptComplete(result);
        }

        @Override
        public void onError(int errCode) {
            Log.d(LOG_TAG, "onError : " + errCode);
            //nativeOnVerifyReceiptError(String.valueOf(errCode));
        }
    };

    public native void nativeOnPaymentComplete(boolean bSuccess, String ProductID, String TxID, String Receipt);

    public native void nativeOnCommandComplete(boolean bSuccess, String[] ProductIDArray, String[] TitleArray, String[] PriceArray);
}