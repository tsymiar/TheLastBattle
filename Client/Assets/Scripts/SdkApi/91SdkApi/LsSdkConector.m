//
//  LsSdkConector.m
//  unityplusfor91sdk
//
//  Created by lisind on 14-3-20.
//  Copyright (c) 2013年 李思. All rights reserved.
//

/*
 *   91SDK常用API,适用于91SDK3.3
 */


#import "LsSdkConector.h"r
#import <NdComPlatform/NdComPlatform.h>

@interface LsSdkConector : NSObject
@property (nonatomic, retain)   NSString*   unityMsgReceiver;
@property (nonatomic, retain)   NSString*   lastLoginedUin;
@end


//代码定义区域 C - Begin
@implementation LsSdkConector

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    self.unityMsgReceiver = nil;
    self.lastLoginedUin = nil;
    [super dealloc];
}

+ (id)sharedInstance {
    static  LsSdkConector*  s_instance = nil;
    if (nil == s_instance) {
        @synchronized(self) {
            if (nil == s_instance) {
                s_instance = [[self alloc] init];
            }
        }
    }
    return s_instance;
}

- (void)sendDictMessage:(NSString *)messageName param:(NSDictionary *)dict
{
    NSString *param = @"";
    for (NSString *key in dict)
    {
        if ([param length] == 0)
        {
            param = [param stringByAppendingFormat:@"%@=%@", key, [dict valueForKey:key]];
        }
        else
        {
            param = [param stringByAppendingFormat:@"&%@=%@", key, [dict valueForKey:key]];
        }
    }
    UnitySendMessage([_unityMsgReceiver UTF8String], [messageName UTF8String], [param UTF8String]);
}

- (void)sendMessage:(NSString *)messageName param:(NSString *)param
{
    UnitySendMessage([_unityMsgReceiver UTF8String], [messageName UTF8String], [param UTF8String]);
}


//初始化SDK
- (void)NdInit:(int)appid appkey:(NSString*)appkey isdebug:(BOOL)isdebug
{
    //添加各种监听
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(InitResult:) name:(NSString *)kNdCPInitDidFinishNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(LoginResult:) name:(NSString *)kNdCPLoginNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(SessionInvalid:) name:(NSString *)kNdCPSessionInvalidNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(PauseExit:) name:(NSString *)kNdCPPauseDidExitNotification object:nil];
    

    //初始化SDK
    NdInitConfigure *cfg = [[[NdInitConfigure alloc]init]autorelease];
    cfg.appid = appid;
    cfg.appKey = appkey;
    //需要检测版本则versionCheckLevel为ND_VERSION_CHECK_LEVEL_STRICT;
    //否则为ND_VERSION_CHECK_LEVEL_NORMAL
    //cfg.versionCheckLevel = ND_VERSION_CHECK_LEVEL_NORMAL;
    cfg.orientation = UIInterfaceOrientationLandscapeLeft;
    //cfg.debugMode = isdebug;
    [[NdComPlatform defaultPlatform] NdInit:cfg];
}

//初始化完成监听
- (void)InitResult:(NSNotification *)notify
{
    [self sendMessage:@"InitResult" param:@""];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kNdCPInitDidFinishNotification object:nil];
}

//账号登陆
- (void)NdLogin
{
    [[NdComPlatform defaultPlatform] NdLogin:0];
}

//游客登陆
- (void)NdLoginEx
{
    [[NdComPlatform defaultPlatform] NdLoginEx:0];
}


//账号登陆监听
- (void)LoginResult:(NSNotification *)notify
{
    if([[NdComPlatform defaultPlatform]isLogined]){
        NSString* loginedUin = [[NdComPlatform defaultPlatform] loginUin];
        bool hasChangedUser = [_lastLoginedUin length] > 0 && ![_lastLoginedUin isEqualToString:loginedUin];
        if(!hasChangedUser){
            [self sendMessage:@"LoginResultSuccess" param:loginedUin];
        }
        else{
            [self sendMessage:@"LoginResultChangeOk" param:loginedUin];
        }
        self.lastLoginedUin = loginedUin;
    }
    else{
        [self sendMessage:@"LoginResultFail" param:@""];
    }
    
}

//显示工具条
- (void)NdShowToolBar:(int)point
{
    [[NdComPlatform defaultPlatform]NdShowToolBar:point];
}

//隐藏工具条
- (void)NdHideToolBar
{
    [[NdComPlatform defaultPlatform]NdHideToolBar];
}

//显示暂停页
- (void)NdShowPause
{
    [[NdComPlatform defaultPlatform] NdPause];
}

//暂停退出监听
- (void)PauseExit:(NSNotification *)notify
{
    [self sendMessage:@"PauseExit" param:@""];
}


//判断是否登陆
- (BOOL)isLogined
{
    return [[NdComPlatform defaultPlatform] isLogined];
}

//返回账号状态：0表示未登陆、1表示游客登陆、2表示普通账号登陆
- (int)getCurrentLoginState
{
    return [[NdComPlatform defaultPlatform] getCurrentLoginState];
}

//返回玩家ID
- (NSString *)loginUin
{
    return [[NdComPlatform defaultPlatform] loginUin];
}

//返回玩家sessionId
- (NSString *)sessionId
{
    return [[NdComPlatform defaultPlatform] sessionId];
}

//返回玩家昵称
- (NSString *)nickName
{
    return [[NdComPlatform defaultPlatform] nickName];
}

//返回玩家详细信息
//- (NSString *) NdGetMyInfo
//{
//    return [[NdComPlatform defaultPlatform] NdGetMyInfo];
//}


//注销,返回错误码,param: 0,表示注销；1，表示注销，并清除自动登录
- (int)NdLogout:(int)param
{
    return [[NdComPlatform defaultPlatform] NdLogout:param];
}

//支付监听
- (void)PayResult:(NSNotification *)notify
{
    NSDictionary *dic = [notify userInfo];
	BOOL bSuccess = [[dic objectForKey:@"result"] boolValue];
	if (!bSuccess) {
		//TODO: 购买失败处理
		int nErrorCode = [[dic objectForKey:@"error"] intValue];
		switch (nErrorCode) {
			case ND_COM_PLATFORM_ERROR_USER_CANCEL:
				//用户取消操作
                [self sendDictMessage:@"PayResultUserCancel" param:dic];
				break;
			case ND_COM_PLATFORM_ERROR_NETWORK_FAIL:
				//网络连接错误
                [self sendDictMessage:@"PayResultNetworkFail" param:dic];
				break;
			case ND_COM_PLATFORM_ERROR_SERVER_RETURN_ERROR:
				//服务端处理失败
                [self sendDictMessage:@"PayResultServerReturnError" param:dic];
				break;
            case ND_COM_PLATFORM_ERROR_ORDER_SERIAL_SUBMITTED:
                //支付订单已提交;
                [self sendDictMessage:@"PayResultOrderSerialSubmitted" param:dic];
				break;
			default:
				//购买过程发生错误
                [self sendDictMessage:@"PayResultOtherError" param:dic];
				break;
		}
	}
	else {
		//TODO: 购买成功处理
        [self sendDictMessage:@"PayResultSuccess" param:dic];
	}
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kNdCPBuyResultNotification object:nil];
}

//同步支付
- (int)NdUniPay:(NSString *)cooOrderSerial productId :(NSString *)productId  productName :(NSString *)productName productPrice:(float)productPrice productCount:(int) productCount payDescription:(NSString *)payDescription
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(PayResult:) name:(NSString *)kNdCPBuyResultNotification object:nil];
    
    NdBuyInfo *buyInfo = [[NdBuyInfo new] autorelease];
    buyInfo.cooOrderSerial = cooOrderSerial;
    buyInfo.productId = productId;  //自定义的产品ID
    buyInfo.productName = productName;  //产品名称
    buyInfo.productPrice = productPrice; //产品现价，价格大等于0.01,支付价格以此为准
    buyInfo.productOrignalPrice = productPrice; //产品原价，同现价保持一致
    buyInfo.productCount = productCount;  //产品数量
    buyInfo.payDescription = payDescription;   //服务器分区，不超过20个字符，只允许英文或数字
    
    //发起请求并检查返回值。注意！该返回值并不是交易结果！
    return [[NdComPlatform defaultPlatform] NdUniPay:buyInfo];
}

//异步支付
- (int)NdUniPayAsyn:(NSString *)cooOrderSerial productId :(NSString *)productId  productName :(NSString *)productName productPrice:(float)productPrice productCount:(int) productCount payDescription:(NSString *)payDescription
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(PayResult:) name:(NSString *)kNdCPBuyResultNotification object:nil];
    
    NdBuyInfo *buyInfo = [[NdBuyInfo new] autorelease];
    buyInfo.cooOrderSerial = cooOrderSerial;
    buyInfo.productId = productId;  //自定义的产品ID
    buyInfo.productName = productName;  //产品名称
    buyInfo.productPrice = productPrice; //产品现价，价格大等于0.01,支付价格以此为准
    buyInfo.productOrignalPrice = productPrice; //产品原价，同现价保持一致
    buyInfo.productCount = productCount;  //产品数量
    buyInfo.payDescription = payDescription;   //服务器分区，不超过20个字符，只允许英文或数字
    
    //发起请求并检查返回值。注意！该返回值并不是交易结果！
    return [[NdComPlatform defaultPlatform] NdUniPayAsyn:buyInfo];
}

//代币充值
- (int)NdUniPayForCoin:(NSString *)cooOrderSerial needPayCoins:(int)needPayCoins payDescription:(NSString *)payDescription
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(LeavePlatform:) name:(NSString *)kNdCPLeavePlatformNotification object:nil];
    return [[NdComPlatform defaultPlatform] NdUniPayForCoin:cooOrderSerial needPayCoins:needPayCoins payDescription:payDescription];
}

//离开平台监听
- (void)LeavePlatform:(NSNotification *)notify
{
    [self sendMessage:@"PayResultOrderSerialSubmitted" param:@""];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kNdCPLeavePlatformNotification object:nil];
}

//会话Session监听
- (void)SessionInvalid:(NSNotification *)notify
{
    [self sendMessage:@"SessionInvalid" param:@""];
}





@end
//代码定义区域 C - End


//代码定义区域 B - Begin
#if defined(__cplusplus)
extern "C"{
#endif
    
    NSString* ConvertToNSString (const char* string)
    {
        if (string)
            return [NSString stringWithUTF8String: string];
        else
            return [NSString stringWithUTF8String: ""];
    }
    
    char* MakeStringCopy(const char* str)
    {
        if(NULL==str)
        {
            return  NULL;
        }
        char* s = (char*)malloc(strlen(str)+1);
        strcpy(s,str);
        return s;
    }
    
    void LsNdInit(const int appid,const char *appkey,const bool isdebug)
    {
        [[LsSdkConector sharedInstance] NdInit:appid appkey:ConvertToNSString(appkey) isdebug:isdebug];
    }
    
    void LsNdLogin()
    {
        [[LsSdkConector sharedInstance] NdLogin];
    }
    
    void LsNdLoginEx()
    {
        [[LsSdkConector sharedInstance] NdLoginEx];
    }
    
    void LsNdShowToolBar(const int point)
    {
        [[LsSdkConector sharedInstance] NdShowToolBar:point];

    }
    
    void LsNdHideToolBar()
    {
        [[LsSdkConector sharedInstance] NdHideToolBar];

    }
    
    void LsNdPause()
    {
        [[LsSdkConector sharedInstance] NdShowPause];
    }
    
    bool LsisLogined()
    {
        return [[LsSdkConector sharedInstance] isLogined];
    }
    
    int LsgetCurrentLoginState()
    {
        return [[LsSdkConector sharedInstance] getCurrentLoginState];
    }
    

    const char* LsloginUin()
    {
        return MakeStringCopy([[[LsSdkConector sharedInstance] loginUin] UTF8String]);
    }
    
    const char* LssessionId()
    {
        return MakeStringCopy([[[LsSdkConector sharedInstance] sessionId] UTF8String]);
    }
    
    const char* LsnickName()
    {
        return MakeStringCopy([[[LsSdkConector sharedInstance] nickName] UTF8String]);
    }
    
    int LsNdLogout(int param)
    {
        return [[LsSdkConector sharedInstance] NdLogout:param];
    }
    
    
    int LSNdUniPay(const char*  cooOrderSerial,const char* productId,const char* productName,float productPrice,int productCount,const char* payDescription)
    {
        return [[LsSdkConector sharedInstance] NdUniPay:ConvertToNSString(cooOrderSerial) productId:ConvertToNSString(productId) productName:ConvertToNSString(productName) productPrice:productPrice productCount:productCount payDescription:ConvertToNSString(payDescription)];

    }
    
    int LsNdUniPayAsyn(const char*  cooOrderSerial,const char* productId,const char* productName,float productPrice,int productCount,const char* payDescription)
    {
        return [[LsSdkConector sharedInstance] NdUniPayAsyn:ConvertToNSString(cooOrderSerial) productId:ConvertToNSString(productId) productName:ConvertToNSString(productName) productPrice:productPrice productCount:productCount payDescription:ConvertToNSString(payDescription)];
        
    }
    
    
    int LsNdUniPayForCoin(const char*  cooOrderSerial,int needPayCoins,const char* payDescription)
    {
        return [[LsSdkConector sharedInstance] NdUniPayForCoin:ConvertToNSString(cooOrderSerial) needPayCoins:needPayCoins payDescription:ConvertToNSString(payDescription)];
        
    }
    
    
    void LsSetUnityReceiver(const char* string)
    {
        [[LsSdkConector sharedInstance] setUnityMsgReceiver:ConvertToNSString(string)];
    }
    

#if defined(__cplusplus)
}
#endif
//代码定义区域 B - End


//代码定义区域 A - Begin
#if defined(__cplusplus)
extern "C"{
#endif
    extern void UnitySendMessage(const char *, const char *, const char *);
    extern NSString* ConvertToNSString (const char* string);
#if defined(__cplusplus)
}
#endif
//代码定义区域 A - End

