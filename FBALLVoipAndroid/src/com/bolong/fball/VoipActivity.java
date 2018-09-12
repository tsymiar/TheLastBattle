package com.bolong.fball;

import java.util.Random;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.socialsdk.SocialManager;
import com.unity3d.player.UnityPlayerActivity;

public class VoipActivity extends UnityPlayerActivity {

	private Handler handler;
	//private Button testBtn;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		
		com.socialsdk.SocialConfig.SUPPORT_MAP_VIEW=false;
		com.socialsdk.SocialConfig.SUPPORT_UMENG=false;
		com.socialsdk.SocialConfig.SUPPORT_FORUM=false;
		com.socialsdk.SocialConfig.SUPPORT_VOICECHAT=true;
		SocialManager.setShowFloat(false);
		SocialManager.startSocialService(VoipActivity.this, new Random().nextInt(1000)+"");
		
		handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				super.handleMessage(msg);
				switch (msg.what) {
				case 1:
					SocialManager.setShowFloat(true);
					SocialManager.startVoiceChat((Long)msg.obj, new com.socialsdk.correspondence.interfaces.CallBack(){
						@Override
						public void onFailed(String arg0) {
							System.out.println("startVoiceChat failed");
						}

						@Override
						public void onSuccessed() {
							System.out.println("startVoiceChat successed");
						}
					
					});
					break;
				case 2:
					SocialManager.stopVoiceChat();
					SocialManager.destroy(VoipActivity.this);
					break;
				default:
					break;
				}
			}
		};
	}
	
	public int startVoiceChat(final long grpId)
	{
		Message message = new Message();
        message.what = 1;
        message.obj = grpId;
        handler.sendMessage(message);
		return 1;
	}
	
	public int stopVoiceChat()
	{
		Message message = new Message();
        message.what = 2;
        handler.sendMessage(message);
		return 1;
	}

}
