using UnityEngine;
using System.Collections;
using GameDefine;
using BlGame.Ctrl;
using BlGame.Resource;
namespace BlGame.View
{
    public class GameSettingWindow : BaseWindow
    {

        public GameSettingWindow()
        {
            mScenesType = EScenesType.EST_Login;
            mResName = GameConstDefine.LoadGameSettingUI;
            mResident = false;
        }
        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GameSettingEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GameSettingExit, Hide);
            EventCenter.AddListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }


        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GameSettingEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GameSettingExit, Hide);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_LobbyExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mSoundBtn = mRoot.FindChild("SoundSwitch").gameObject;
            mMusicBtn = mRoot.FindChild("MusicSwitch").gameObject;
            mEffectBtn = mRoot.FindChild("EffectSwitch").gameObject;
            mRangeBtn = mRoot.FindChild("RangeSwitch").gameObject;
            mCloseBtn = mRoot.FindChild("CloseBtn").gameObject;

            mExchangeBtn = mRoot.FindChild("ExchangeBtn").GetComponent<UIButton>();

            mObjOn[0] = mSoundBtn.transform.FindChild("On").gameObject;
            mObjOff[0] = mSoundBtn.transform.FindChild("Off").gameObject;
            mObjOn[1] = mMusicBtn.transform.FindChild("On").gameObject;
            mObjOff[1] = mMusicBtn.transform.FindChild("Off").gameObject;

            mObjOn[2] = mEffectBtn.transform.FindChild("On").gameObject;
            mObjOff[2] = mEffectBtn.transform.FindChild("Off").gameObject;
            mObjOn[3] = mRangeBtn.transform.FindChild("On").gameObject;
            mObjOff[3] = mRangeBtn.transform.FindChild("Off").gameObject;

            UIEventListener.Get(mCloseBtn.gameObject).onClick += ClosePress;
            UIEventListener.Get(mSoundBtn.gameObject).onClick += SoundPress;
            UIEventListener.Get(mMusicBtn.gameObject).onClick += MusicPress;
            UIEventListener.Get(mEffectBtn.gameObject).onClick += EffectPress;
            UIEventListener.Get(mRangeBtn.gameObject).onClick += RangePress;
            EventDelegate.Add(mExchangeBtn.onClick, ExChangePress);

            SetSave(ref SoundOpenState, UIGameSetting.soundKey, SystemSet.SoundButton);
            SetSave(ref MusicOpenState, UIGameSetting.voiceKey, SystemSet.MusicButton);
            SetSave(ref EffectOpenState, GameMethod.EffectVoc, SystemSet.EffectButton);
            SetSave(ref RangeOpenState, GameMethod.RangeVoc, SystemSet.RangeButton);
        }

        private void ExChangePress()
        {
            ExtraBonusCtrl.Instance.Enter();
        }

        private void RangePress(GameObject go)
        {
            RangeOpenState = !RangeOpenState;
            Save(ref RangeOpenState, GameMethod.RangeVoc, SystemSet.RangeButton);
        }

        private void EffectPress(GameObject go)
        {
            EffectOpenState = !EffectOpenState;
            Save(ref EffectOpenState, GameMethod.EffectVoc, SystemSet.EffectButton);
        }

        private void MusicPress(GameObject go)
        {
            MusicOpenState = !MusicOpenState;
            Save(ref MusicOpenState, UIGameSetting.voiceKey, SystemSet.MusicButton);
        }

        private void SoundPress(GameObject go)
        {
            SoundOpenState = !SoundOpenState;
            Save(ref SoundOpenState, UIGameSetting.soundKey, SystemSet.SoundButton);
        }
        void Save(ref bool saveState, string key, SystemSet systemSet)
        {
            mObjOn[(int)systemSet].SetActive(saveState);
            mObjOff[(int)systemSet].SetActive(!saveState);
            int state = saveState ? 1 : 0;
            PlayerPrefs.SetInt(key, state);

            switch (systemSet)
            {
                case SystemSet.SoundButton:
                    AudioManager.Instance.EnableSound(saveState);
                    break;
                case SystemSet.MusicButton:
                    AudioManager.Instance.EnableVoice(saveState);
                    break;
                case SystemSet.EffectButton:
                    BattleInfoCtrl.Instance.SetEffect(saveState);
                    break;
                case SystemSet.RangeButton:
                    GameMethod.SetRange(saveState);
                    break;
            }
        }

        void SetSave(ref bool saveState, string key, SystemSet systemSet)
        {
            int state = 0;
            if (PlayerPrefs.HasKey(key))
            {
                state = PlayerPrefs.GetInt(key);
                saveState = (state == 1) ? true : false;
            }
            else
            {
                state = saveState ? 1 : 0;
                PlayerPrefs.SetInt(key, state);
            }
            Save(ref saveState, key, systemSet);
        }

        private void ClosePress(GameObject go)
        {
            GameSettingCtrl.Instance.Exit();
        }


        protected override void RealseWidget()
        {

        }


        public override void OnEnable()
        {

        }
        //隐藏
        public override void OnDisable()
        {

        }

        //游戏事件注册
        protected override void OnAddListener()
        {

        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {

        }
        GameObject mCloseBtn;
        GameObject mSoundBtn;
        GameObject mMusicBtn;
        GameObject mEffectBtn;
        GameObject mRangeBtn;

        UIButton mExchangeBtn;
        

        GameObject[] mObjOn = new GameObject[4];
        GameObject[] mObjOff = new GameObject[4];

        private bool SoundOpenState = true;
        private bool MusicOpenState = true;
        private bool EffectOpenState = true;
        private bool RangeOpenState = true;
    }
}