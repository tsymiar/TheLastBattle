using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using BlGame.GameEntity;
using System.Linq;
using BlGame;
namespace BlGame.View
{
    public class EnemyWindow : BaseWindow
    {
        enum EnemyInfo
        {
            ENEMY_ONE,
            ENEMY_TWO,
            ENEMY_THR
        }

        public EnemyWindow() 
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.LoadEnemyUI;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayEnter, Show);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            Transform GameObj = mRoot.Find("EnemyTeamManager");
            int num = GameObj.childCount;
            for (int i = 0; i < num; i++)
            {
                ButtonOnPress btn = GameObj.GetChild(i).GetComponent<ButtonOnPress>();
                btn.AddListener(i, EnemyTeamSelectFunc);
                mEnemyTeamList.Add(new EnemyHeadInfo(btn));
            }
        }

        //窗口控件释放
        protected override void RealseWidget()
        {
            mEnemyTeamList.Clear();
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener<CEvent>(EGameEvent.eGameEvent_AddOrDelEnemy, OnEvent);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener<CEvent>(EGameEvent.eGameEvent_AddOrDelEnemy, OnEvent);
        }

        //显示
        public override void OnEnable()
        {
          
        }

        //隐藏
        public override void OnDisable()
        {
        }

        public override void Update(float deltaTime) 
        {
            if (Time.frameCount % 5 != 0)
            {
                return;
            }
            //check player info distance up lockDistance;
            Iselfplayer player = PlayerManager.Instance.LocalPlayer;
            foreach (var item in mEnemyTeamList)
            {
                if (item.Player == null || item.Player.realObject == null)
                    continue;

                float dis = Vector3.Distance(item.Player.realObject.transform.position, player.realObject.transform.position);
                if (item.Player.FSM.State == BlGame.FSM.FsmState.FSM_STATE_DEAD || dis > GameConstDefine.PlayerLockTargetDis || item.Player.realObject.activeSelf == false)
                {
                    item.CloseInfo();
                }
                else
                {
                    item.OnUpdateHp();
                    if (item.Player == player.SyncLockTarget)
                    {
                        item.OnSelect(true);
                    }
                    else
                    {
                        item.OnSelect(false);
                    }
                }
            }
        }

        void OnEvent(CEvent eve)
        {
            Iplayer entity = (Iplayer)eve.GetParam("Target");
            bool add = (bool)eve.GetParam("Add");
            if (add)
            {
                AddPlayerEnemy(entity);
            }
            else
            {
                RemovePlayerEnemy(entity);
            }
        }

        private void AddPlayerEnemy(Iplayer pl)
        {
            EnemyHeadInfo info = GetEmptyHeadInfo(pl);

            if (info == null)
            {
                //foreach(var item in EnemyTeamList)
                //    item.OnSelect(false);
                return;
            }
            //   RemovePlayerEnemy(pl);
            info.OpenInfo(pl);
            info.OnUpdateHp();
            if (PlayerManager.Instance.LocalPlayer.SyncLockTarget == null)
                return;
            if (PlayerManager.Instance.LocalPlayer.SyncLockTarget.GameObjGUID != info.Player.GameObjGUID)
            {
                return;
            }
            EnemySelect(info, true);
        }

        void EnemySelect(EnemyHeadInfo info, bool select)
        {
            info.OnSelect(select);
            //foreach (EnemyHeadInfo hf in EnemyTeamList) {
            //    if(hf == info && info != null){
            //        hf.OnSelect(true);
            //    }else{
            //        hf.OnSelect(false);
            //    }	
            //}
        }

        private bool IsEnemyHeadEmpty()
        {
            foreach (EnemyHeadInfo team in mEnemyTeamList)
            {
                if (team.Player != null)
                {
                    return false;
                }
            }
            return true;
        }

        bool HasPlayerHead(Iplayer pl)
        {
            if (pl == null) return true;
            foreach (EnemyHeadInfo team in mEnemyTeamList)
            {
                if (team.Player == pl)
                    return true;
            }
            return false;
        }

        private EnemyHeadInfo GetEmptyHeadInfo(Iplayer pl)
        {
            if (HasPlayerHead(pl))
                return null;

            if (PlayerManager.Instance.LocalPlayer.EntityCamp == pl.EntityCamp)
                return null;
            foreach (EnemyHeadInfo team in mEnemyTeamList)
            {
                if (team.Player == null)
                {
                    //	team.OnSelect(false);
                    return team;
                }
            }
            return null;
        }

        private void RemovePlayerEnemy(Iplayer pl)
        {
            if (pl == null) return;
            for (int i = mEnemyTeamList.Count - 1; i >= 0; i--)
            {
                if (mEnemyTeamList.ElementAt(i).Player == pl)
                {
                    mEnemyTeamList.ElementAt(i).CloseInfo();
                    break;
                }
            }
        }

        private void EnemyTeamSelectFunc(int index, bool pressed)
        {
            if (pressed)
            {
                return;
            }
            if (mEnemyTeamList[index].Player == null)
            {
                return;
            }
            switch ((EnemyInfo)index)
            {
                case EnemyInfo.ENEMY_ONE:
                case EnemyInfo.ENEMY_TWO:
                case EnemyInfo.ENEMY_THR:
                    PlayerManager.Instance.LocalPlayer.SetSyncLockTarget(mEnemyTeamList[index].Player);
                    break;
            }
            Debug.Log("EnemyTeamSelectFunc  " + index);
        }

        private List<EnemyHeadInfo> mEnemyTeamList = new List<EnemyHeadInfo>();
    }

    public class EnemyHeadInfo
    {
        public UISprite HpSlider;
        public UISprite SelectSprite;
        public UISprite ShowSprite;
        public Iplayer Player
        {
            private set;
            get;
        }
        UITweener uit;
        private Transform teamPress;
        Vector3 orignalPos = new Vector3();
        public EnemyHeadInfo(ButtonOnPress team)
        {
            teamPress = team.transform;
            ShowSprite = team.transform.FindChild("SpriteShow").GetComponent<UISprite>();
            SelectSprite = team.transform.FindChild("SpriteSelect").GetComponent<UISprite>();
            HpSlider = team.transform.FindChild("Progress Bar").FindChild("Foreground").GetComponent<UISprite>();
            //ShowSprite.gameObject.SetActive (false);
            SelectSprite.gameObject.SetActive(false);
            orignalPos = team.transform.localPosition;
            TweenPosition.Begin(teamPress.gameObject, 0f, new Vector3(orignalPos.x + 110f, orignalPos.y, orignalPos.z));
            //HpSlider.gameObject.SetActive (false);
            teamPress.gameObject.SetActive(false);
        }

        public void OnUpdateHp()
        {

            if (Player == null)
            {
                return;
            }
            HpSlider.fillAmount = Player.Hp / Player.HpMax;
            HpSlider.gameObject.SetActive(true);
        }

        public void OnSelect(bool isVis)
        {
            SelectSprite.gameObject.SetActive(isVis);
        }

        public void OpenInfo(Iplayer pl)
        {
            Player = pl;
            //ShowSprite.gameObject.SetActive (true);
            HeroSelectConfigInfo heroinfo = ConfigReader.GetHeroSelectInfo(Player.NpcGUIDType);
            TweenPosition.Begin(teamPress.gameObject, 0.2f, new Vector3(orignalPos.x, orignalPos.y, orignalPos.z));
            ShowSprite.spriteName = heroinfo.HeroSelectHead.ToString();
            teamPress.gameObject.SetActive(true);

            if (uit != null)
                EventDelegate.Remove(uit.onFinished, Finished);
        }

        public void CloseInfo()
        {
            Player = null;
            uit = TweenPosition.Begin(teamPress.gameObject, 0.5f, new Vector3(orignalPos.x + 110f, orignalPos.y, orignalPos.z));
            SelectSprite.gameObject.SetActive(false);
            EventDelegate.Add(uit.onFinished, Finished);
        }
        void Finished()
        {
            EventDelegate.Remove(uit.onFinished, Finished);
            teamPress.gameObject.SetActive(false);
        }
    }
}