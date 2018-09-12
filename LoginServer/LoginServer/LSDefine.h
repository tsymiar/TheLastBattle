#ifndef _LS_DEFINE_H
#define _LS_DEFINE_H
#endif

//Sdk Message
//91_ios
const string appid_91 = "115561";														//同客户端一致的APPID
const string appkey_91 = "5fd1729c881082a107591fb88234c6b94180816ffd5abffa";				//同客户端一致的APPKEY
const string goUrl_91 = "http://service.sj.91.com/usercenter/AP.aspx?";					//平台服务器地址 get模式发送

//PP_iOS
const string appid_pp = "4069";
const string appkey_pp = "4b1386357ffc6f063dc32a763fedba09";
const string goUrl_pp = "http://passport_i.25pp.com:8080/account?tunnel-command=2852126760";	//post 

//TB_iOS
const string appid_tb = "140820";
const string appkey_tb = "YNlxI@Kh5u7Ro2qAYkax@UJhtE7od1qX";
const string goUrl_tb1 = "http://tgi.tongbu.com/check.aspx?";		//get	
const string goUrl_tb2 = "http://tgi.tongbu.com/checkv2.aspx?";		//get

//CMGE_iOS_越狱
const string gameid_cmge_JB = "D10080";						//product_id
const string serverid_cmge_JB = "M1098";						
const string appkey_cmge_JB = "640ro08insrj7x717q16";	//client_id
const string gamekey_cmge_JB = "w3my7nnlvd3b0skrg0d2";		//secret_key
const string synkey_cmge_JB = "f2pyef52wvrj1u8hx6s4";			
const string projectID_cmge_JB = "P11290";
//CMGE_iOS_正版
const string gameid_cmge_ZB = "D10081";
const string serverid_cmge_ZB = "M1099";
const string appkey_cmge_ZB = "42ygskk1vguw0nx706kk";
const string gamekey_cmge_ZB = "k5rkew5jogoox9nqnd6u";
const string synkey_cmge_ZB = "b6vgm167cc2nv1av17bb";
const string projectID_cmge_ZB = "P11291";
//CMGE_android
const string gameid_cmge_android = "D10106A";
const string serverid_cmge_android = "M1138A";
const string appkey_cmge_android = "pkf15u5pkc19j08s3p4i";
const string gamekey_cmge_android = "7cij2vyf17vpzocugo70";
const string synkey_cmge_android = "1gl97w6keq711of944fb";
const string projectID_cmge_android = "P14969A";

const string messageId_cmge = "77";
const string grant_type_cmge = "authorization_code";
//const string cmge_header = "application/x-www-form-urlencoded";
const string goUrl_cmge_login = "http://iosrs.tisgame.com/sdksrv/auth/getToken.lg";				//post
const string goUrl_cmge_info =  "http://iosrs.tisgame.com/sdksrv/auth/getUserInfo.lg";			//post

//UC_iOS
const string uc_cpId = "109";
const string gameId_uc = "123456";
const string appkey_uc = "202cb962234w4ers2aaa";
const string goUrl_uc = "http://sdk.g.uc.cn/ss/bp/ucid";		//post
//const string goUrl_uc = "http://sdk.test4.g.uc.cn/ss";

//iTools_iOS
const string appid_iTools = "418";
const string appkey_iTools = "1D7473C7403084C618EF28B825F6F325";
const string goUrl_iTools = "https://pay.slooti.com/?r=auth/verify";  //get


//As_iOS
const string appkey_As = "3d34950737c047f3b854fad0821551ef";
const string appid_As = "180";
const string goUrl_As = "https://pay.i4.cn/member_third.action?";

//OnlineGame_iOS
const string appkey_OnlineGame = "";
const string goUrl_OnlineGame = "http://f_signin.bppstore.com/loginCheck.php"; //post or get(get perfect)

//XY
const string appid_XY = "100004696";
const string appkey_XY = "AzTD4KuQY2lHUmmBUNrbft2H7n9scEEU";
const string goUrl_XY = "http://passport.xyzs.com/checkLogin.php";		//post
