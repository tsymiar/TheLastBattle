using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public class TDGAVirtualCurrency {
#if UNITY_IPHONE
	[DllImport ("__Internal")]
	private static extern void _tdgaOnChargeRequst(string orderId, string iapId, double currencyAmount, 
			string currencyType, double virtualCurrencyAmount, string paymentType);
	
	[DllImport ("__Internal")]
	private static extern void _tdgaOnChargSuccess(string orderId);
	
	[DllImport ("__Internal")]
	private static extern void _tdgaOnReward(double virtualCurrencyAmount, string reason);
#elif UNITY_ANDROID
	private static string JAVA_CLASS = "com.tendcloud.tenddata.TDGAVirtualCurrency";
	static AndroidJavaClass agent = new AndroidJavaClass(JAVA_CLASS);
#endif
	
	public static void OnChargeRequest(string orderId, string iapId, double currencyAmount,
			string currencyType, double virtualCurrencyAmount, string paymentType) {
		//if the platform is real device
		if (Application.platform != RuntimePlatform.OSXEditor && Application.platform != RuntimePlatform.WindowsEditor) {
#if UNITY_IPHONE
			_tdgaOnChargeRequst(orderId, iapId, currencyAmount, currencyType, virtualCurrencyAmount, paymentType);
#elif UNITY_ANDROID
			agent.CallStatic("onChargeRequest", orderId, iapId, currencyAmount, 
					currencyType, virtualCurrencyAmount, paymentType);
#elif UNITY_WP8
			TalkingDataGAWP.TDGAVirtualCurrency.onChargeRequest(orderId, iapId, currencyAmount, 
					currencyType, virtualCurrencyAmount, paymentType);
#endif
		}
	}
	
	public static void OnChargeSuccess(string orderId) {
		//if the platform is real device
		if (Application.platform != RuntimePlatform.OSXEditor && Application.platform != RuntimePlatform.WindowsEditor) {
#if UNITY_IPHONE
			_tdgaOnChargSuccess(orderId);
#elif UNITY_ANDROID
			agent.CallStatic("onChargeSuccess", orderId);
#elif UNITY_WP8
			TalkingDataGAWP.TDGAVirtualCurrency.onChargeSuccess(orderId);
#endif
		}
	}
	
	public static void OnReward(double virtualCurrencyAmount, string reason) {
		//if the platform is real device
		if (Application.platform != RuntimePlatform.OSXEditor && Application.platform != RuntimePlatform.WindowsEditor) {
#if UNITY_IPHONE
			_tdgaOnReward(virtualCurrencyAmount, reason);
#elif UNITY_ANDROID
			agent.CallStatic("onReward", virtualCurrencyAmount, reason);
#elif UNITY_WP8
			TalkingDataGAWP.TDGAVirtualCurrency.onReward(virtualCurrencyAmount, reason);
#endif
		}
	}
}
