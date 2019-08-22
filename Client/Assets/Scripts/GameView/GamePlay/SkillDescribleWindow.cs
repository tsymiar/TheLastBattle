using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic; 
using BlGame.GameEntity;
using GameDefine;
using JT.FWW.Tools;
using BlGame.Resource;

namespace BlGame.View
{
    public class SkillDescribleWindow : BaseWindow
    {
        public SkillDescribleWindow()
        {
            mScenesType = EScenesType.EST_Play;
            mResName = GameConstDefine.SkillDestribe;
            mResident = false;
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化
        public override void Init()
        {
            EventCenter.AddListener<bool,  SkillType,  Iplayer >(EGameEvent.eGameEvent_SkillDescribleType, ShowDescribleByType);
            EventCenter.AddListener<bool, int, Iplayer>(EGameEvent.eGameEvent_SkillDescribleId, ShowDescribleById);
            EventCenter.AddListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //类对象释放
        public override void Realse()
        {
            EventCenter.RemoveListener<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, ShowDescribleByType);
            EventCenter.RemoveListener<bool, int, Iplayer>(EGameEvent.eGameEvent_SkillDescribleId, ShowDescribleById);
            EventCenter.RemoveListener(EGameEvent.eGameEvent_GamePlayExit, Hide);
        }

        //窗口控件初始化
        protected override void InitWidget()
        {
            mSkillCd = mRoot.FindChild("Skill_Cooldown").GetComponent<UILabel>();

            mSkillDes = mRoot.FindChild("Skill_Describe").GetComponent<UILabel>();

            mSkillLv = mRoot.FindChild("Skill_Level").GetComponent<UILabel>();

            mSkillName = mRoot.FindChild("Skill_Name").GetComponent<UILabel>();

            mSkillDis = mRoot.FindChild("Skill_Distance").GetComponent<UILabel>();

            mSkillMpCost = mRoot.FindChild("Skill_MP").GetComponent<UILabel>();

            mSkillHpCost = mRoot.FindChild("Skill_HP").GetComponent<UILabel>();

            mSkillCpCost = mRoot.FindChild("Skill_CP").GetComponent<UILabel>();
        }

        //窗口控件释放
        protected override void RealseWidget()
        { 
        }

        //游戏事件注册
        protected override void OnAddListener()
        {
            EventCenter.AddListener(EGameEvent.eGameEvent_SkillDescribleUpdate, UpdateDescrible);
        }

        //游戏事件注消
        protected override void OnRemoveListener()
        {
            EventCenter.RemoveListener(EGameEvent.eGameEvent_SkillDescribleUpdate, UpdateDescrible);
        }

        //显示
        public override void OnEnable()
        {
            currentSkill = SkillType.SKILL_NULL;
        }

        //隐藏
        public override void OnDisable()
        {

        }


        public override void Update(float deltaTime)
        {
        
        }
       
        public SkillType currentSkill
        {
            private set;
            get;
        }

        private UILabel mSkillName = null;
        private UILabel mSkillDes = null;
        private UILabel mSkillCd = null;
        private UILabel mSkillDis = null;
        private UILabel mSkillLv = null;
        private UILabel mSkillMpCost = null;
        private UILabel mSkillCpCost = null;
        private UILabel mSkillHpCost = null;


        private void ShowDescribleByType(bool show, SkillType skillType, Iplayer player)
        {
            if (show)
            {
                Show();
                SetSkillDestribe(skillType, (Iselfplayer)player);
                currentSkill = skillType;
            }
            else
            {
                Hide();
                currentSkill = SkillType.SKILL_NULL;
            }
        }

        public void ShowDescribleById(bool show, int skillId, Iplayer player)
        {
            if (show)
            {
                Show();
                SetSkillDestribe(skillId, player);
            }
            else
            {
                Hide();
                currentSkill = SkillType.SKILL_NULL;
            }
        }

        private void UpdateDescrible()
        {
            if (currentSkill != SkillType.SKILL_NULL)
            {
                UpdateDestribe(currentSkill, PlayerManager.Instance.LocalPlayer);
            }
        }


        public void UpdateDestribe(SkillType skillType, Iselfplayer player)
        {
            int skillId = player.SkillIdDic[skillType];

            UpdateDestribe(skillId, (Iplayer)player);

        }

        public void UpdateDestribe(int skillId, Iplayer player)
        {
            SkillManagerConfig skillconfig = ConfigReader.GetSkillManagerCfg(skillId);
            if (skillconfig == null)
            {
                return;
            }

            mSkillCd.text = (skillconfig.coolDown / 1000f).ToString();
            mSkillDes.text = DestribeWithAttribue(skillconfig.info, player);

            int bet = skillconfig.id % 10;
            if (bet == 0)
                bet = 1;
            mSkillLv.text = bet.ToString();

            mSkillName.text = skillconfig.name;

            mSkillDis.text = "(" + skillconfig.range.ToString() + "施法距离)";
            ShowCost(skillconfig);

        }


        void ShowCost(SkillManagerConfig skillconfig)
        {
            mSkillHpCost.transform.gameObject.SetActive(false);
            mSkillCpCost.transform.gameObject.SetActive(false);
            mSkillMpCost.transform.gameObject.SetActive(false);

            if (skillconfig.mpUse != 0)
            {
                mSkillMpCost.text = skillconfig.mpUse.ToString();
                mSkillMpCost.transform.gameObject.SetActive(true);
            }
            else if (skillconfig.hpUse != 0)
            {
                mSkillHpCost.text = skillconfig.hpUse.ToString();
                mSkillHpCost.transform.gameObject.SetActive(true);
            }
            else if (skillconfig.cpUse != 0)
            {
                mSkillCpCost.text = skillconfig.cpUse.ToString();
                mSkillCpCost.transform.gameObject.SetActive(true);
            }
        }


        public void SetSkillDestribe(SkillType skillType, Iselfplayer player)
        {
            if (!player.SkillIdDic.ContainsKey(skillType) || !player.BaseSkillIdDic.ContainsKey(skillType))
                return;
            int skillId = player.SkillIdDic[skillType];

            SetSkillDestribe(skillId, (Iplayer)player);
        }

        public void SetSkillDestribe(int skillId, Iplayer player)
        {
            SkillManagerConfig skillconfig = ConfigReader.GetSkillManagerCfg(skillId);
            if (skillconfig == null)
            {
                return;
            }
            UpdateDestribe(skillId, player);
        }

        string DestribeWithAttribue(string str, Iplayer player)
        {
            string tempStr = "";
            tempStr = str;
            if (!(str.Contains("mag") || str.Contains("phy")))
                return str;
            for (int i = 0; i < tempStr.Length; i++)
            {
                if (tempStr[i] != ']')
                {
                    continue;
                }
                int index = tempStr.LastIndexOf('[', i);
                string addStr = tempStr.Substring(index, i - index + 1);
                string[] strArray;
                if (addStr.Contains("mag") || addStr.Contains("phy"))
                {
                    strArray = addStr.Split(',');
                    strArray[1] = strArray[1].Remove(strArray[1].Length - 1, 1);
                    float attr = float.Parse(strArray[1]);
                    if (player == null)
                    {
                        return null;
                    }
                    float phyAttr = player.PhyAtk;
                    float magAttr = player.MagAtk;
                    attr = addStr.Contains("mag") ? (attr * magAttr) : (attr * phyAttr);
                    string attrAdd = attr >= 0 ? ("+" + attr.ToString()) : ("-" + attr.ToString());
                    attrAdd = "[00FF00]" + attrAdd + "[-]";
                    tempStr = tempStr.Replace(addStr, attrAdd);
                }

            }
            return tempStr;
        }

    }
}



