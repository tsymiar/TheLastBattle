using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameDefine;
using System.Linq;
using JT.FWW.Tools;
using BlGame.GameEntity;
using BlGame.GameData;
using BlGame.Resource;

public class UIViewerSkillInfo : MonoBehaviour {

	public static UIViewerSkillInfo Instance
	{
		private set;
		get;
	}

	void Awake()
	{
		
	}

    void OnEnable()
    {
        Instance = this;
        Init();
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_AbsSkillInfo, AddHeroAbsorbSkill);
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_SkillInfo, InitHeroSkills);
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_SkillUpLvChange, SkillUpgrade);
        EventCenter.AddListener<Iplayer>(EGameEvent.eGameEvent_SkillCDInfo, ChangeSkillCd);

    }
    void OnDisable()
    {
        Instance = null;
        EventCenter.RemoveListener<Iplayer>(EGameEvent.eGameEvent_AbsSkillInfo, AddHeroAbsorbSkill);
        EventCenter.RemoveListener<Iplayer>(EGameEvent.eGameEvent_SkillInfo, InitHeroSkills);
        EventCenter.RemoveListener<Iplayer>(EGameEvent.eGameEvent_SkillUpLvChange, SkillUpgrade);
        EventCenter.RemoveListener<Iplayer>(EGameEvent.eGameEvent_SkillCDInfo, ChangeSkillCd);
    }
    //skill slot
    class SkillSlot
    {
        public int id;
        public UISprite SkillPlayerIcon;
        public CdCountDown cdCount;
        GameObject furyEffect = null;
        Transform transfor = null;
		public SkillSlot(Transform tran, int index )
        {
            id = index;
            transfor = tran;
            SkillPlayerIcon = tran.FindChild("CutBar_" + index).GetComponent<UISprite>();
            cdCount = tran.GetComponent<CdCountDown>();
            SkillPlayerIcon.spriteName = "";
        }
        public void SetVib(bool isVib)
        {
            transfor.gameObject.SetActive(isVib);
        }

        public void SetShowSkill(SkillInfo skill)
        {
            SkillPlayerIcon.spriteName = skill.mPic;

            cdCount.StartCdCountDown(skill.mTotalCD, skill.mLastCD);

            SetVib(skill.mVisible);
        }

       public  void SetFuryEffect()
       {
           if (null == furyEffect)
           {
               //furyEffect = GameObject.Instantiate(Resources.Load(GameDefine.GameConstDefine.FurySkillBtnEffect)) as GameObject;
               ResourceUnit furyEffectUnit = ResourcesManager.Instance.loadImmediate(GameDefine.GameConstDefine.FurySkillBtnEffect, ResourceType.PREFAB);

               furyEffect = GameObject.Instantiate(furyEffectUnit.Asset) as GameObject;
               furyEffect.transform.parent = SkillPlayerIcon.transform;
               furyEffect.transform.localScale = Vector3.one;
               furyEffect.transform.localPosition = Vector3.zero;
           }
        }
       public void DelFuryEffect()
       {
           if (null != furyEffect)
           {
               DestroyImmediate(furyEffect);
               furyEffect = null;
           }
       }
       public void SetDis()
       {
           //SkillPlayerIcon.transform.parent.gameObject.SetActive(false);
       }

	   public void Reset()
	   {
			SkillPlayerIcon.spriteName = "";
			cdCount.EndCdCountDown ();

            if (id == 2 || id == 3)
                SetVib(false);
            else
                SetVib(true);
	   }

    }

    //skill info for ob
    class SkillInfo
    {
        public int mSkillId;
        public int mSlot;
        public string mPic;
        public float mTotalCD;
        public float mLastCD;
        public bool mVisible;

        public SkillInfo(int skillId, int slot, string pic, float totalTime, float lastTime, bool visible)
        {
            mSkillId = skillId;
            mSlot = slot;
            mPic = pic;
            mTotalCD = totalTime;
            mLastCD = lastTime;
            mVisible = visible;
        }
    }

    //hero skill list
    class SkillInfoDate
    {
        public Dictionary<int, SkillInfo> skills = new Dictionary<int, SkillInfo>();

        public SkillInfoDate()
        {
        }

        public void AddSkillInfoDate(int skillId, int slot, string pic, float totalTime, float lastTime, bool visible)
        {
            if (!skills.ContainsKey(skillId))
            {
                SkillInfo skill = new SkillInfo(skillId, slot, pic, totalTime, lastTime, visible);
                skills[skillId] = skill;
            }
        }

        public void RemoveSkillInofDate(int slot)
        {
            int skillId = GetSkillId(slot);

            if (skillId != 0)
            {
                skills.Remove(skillId);
            }
        }

		public void SkillUpgrade(int slot,int skillId, float totalTime)
		{
			SkillInfo skill = GetSkillInfo (slot);

			if (skill == null) 
			{
				return;
			}

			int oldSkillId = skill.mSkillId;

			skill.mSkillId = skillId;
			skill.mTotalCD = totalTime;

			skills.Remove (oldSkillId);

			skills[skillId] = skill;

		}

        public void ChangeColDownTime(int skillId, float lastTime)
        {
            if (skills.ContainsKey(skillId))
            {
                skills[skillId].mLastCD = lastTime;
            }
        }

        public void ChangeVisable(int slot, bool visable)
        {
            int skillId = GetSkillId(slot);
            if (skills.ContainsKey(skillId))
            {
                skills[skillId].mVisible = visable;
            }
        }

        public SkillInfo GetSkillInfo(int slot)
        {
            int skillId = GetSkillId(slot);
            if (skills.ContainsKey(skillId))
            {
                return skills[skillId];
            }

            return null;
        }

        public int GetSkillId(int slot)
        {
            foreach (SkillInfo skill in skills.Values)
            {
                if (skill.mSlot == slot)
                {
                    return skill.mSkillId;
                }
            }

            return 0;
        }

        public void Update(float deltaTime)
        {
            int[] keys = skills.Keys.ToArray();

            for (int i = 0; i < skills.Count; i++)
            {
                skills[keys[i]].mLastCD -= deltaTime;

                if (skills[keys[i]].mLastCD < 0f)
                    skills[keys[i]].mLastCD = 0f;
            }
        }
    }


    //skill slot list
	List<SkillSlot> skillSlots = new List<SkillSlot>();

	//hero skill data
    Dictionary<Iplayer, SkillInfoDate> skillDic = new Dictionary<Iplayer, SkillInfoDate>();

    void Init()
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            Transform tran = transform.FindChild("Button_" + i );
            SkillSlot skillSlot = new SkillSlot(tran, i);
            skillSlots.Add(skillSlot);
            ButtonOnPress button = tran.GetComponent<ButtonOnPress>();
            button.AddListener(i, OnPress);

            if (i == 2 || i == 3)
            {
                skillSlot.SetFuryEffect();
            }
        }
    }



    //change fury state
    public void  ChangeFuryState(Iplayer player)
    {
        EFuryState state = PlayerSkillData.Instance.FuryState;
        if (skillDic.ContainsKey(player))
        {
            if(state == EFuryState.eFuryRunState)
            {
                skillDic[player].ChangeVisable(0, false);
                skillDic[player].ChangeVisable(1, false);
                skillDic[player].ChangeVisable(2, true);
                skillDic[player].ChangeVisable(3, true);
            }
            else
            {
                skillDic[player].ChangeVisable(0, true);
                skillDic[player].ChangeVisable(1, true);
                skillDic[player].ChangeVisable(2, false);
                skillDic[player].ChangeVisable(3, false);
            }
        }

        if (UIViewerPersonInfo.Instance.SetCurrClickPlayer == player) 
		{
			InfoPlayerSkill(player);
		}
    }

    //normal skill init
    public void InitHeroSkills(Iplayer player)
    {
        if (skillDic.ContainsKey(player))
            return;
        SkillInfoDate skillListData = new SkillInfoDate();

        HeroConfigInfo heroConfig;

        if (ConfigReader.HeroXmlInfoDict.TryGetValue(player.NpcGUIDType, out heroConfig))
        {
            SkillManagerConfig skillCfg = ConfigReader.GetSkillManagerCfg(heroConfig.HeroSkillType1);
            if (skillCfg != null)
            {
                skillListData.AddSkillInfoDate(skillCfg.id, 0, skillCfg.skillIcon, skillCfg.coolDown / 1000, 0, true);
            }

            skillCfg = ConfigReader.GetSkillManagerCfg(heroConfig.HeroSkillType2);
            if (skillCfg != null)
            {
                skillListData.AddSkillInfoDate(skillCfg.id, 1, skillCfg.skillIcon, skillCfg.coolDown / 1000, 0, true);
            }

            skillCfg = ConfigReader.GetSkillManagerCfg(heroConfig.HeroSkillType3);
            if (skillCfg != null)
            {
                skillListData.AddSkillInfoDate(skillCfg.id, 2, skillCfg.skillIcon, skillCfg.coolDown / 1000, 0, false);
            }

            skillCfg = ConfigReader.GetSkillManagerCfg(heroConfig.HeroSkillType4);
            if (skillCfg != null)
            {
				skillListData.AddSkillInfoDate(skillCfg.id, 3, skillCfg.skillIcon, skillCfg.coolDown / 1000, 0, false);
            }
        }

        skillDic[player] = skillListData;
    }

    //absorb skill add
    public void AddHeroAbsorbSkill(Iplayer player)
    {
        int skillID = PlayerSkillData.Instance.SkillId;
        int slot = PlayerSkillData.Instance.Slot;
        int skillID1 = PlayerSkillData.Instance.SkillId1;
        int slot1 = PlayerSkillData.Instance.Slot1;
        if (!skillDic.ContainsKey(player))
            return;
        SetAbsVis(player, skillID,slot);
        SetAbsVis(player, skillID1, slot1);
        if (UIViewerPersonInfo.Instance.SetCurrClickPlayer == player) 
		{
            InfoPlayerSkill(player);
		}
    }
    void SetAbsVis(Iplayer player, int skillID,int slot)
    {
        if (skillID == 0)
        {
            skillDic[player].RemoveSkillInofDate(slot);

            if (UIViewerPersonInfo.Instance.SetCurrClickPlayer == player)
            {
                InfoPlayerSkill(player);
            }
            return;
        }
        NpcConfigInfo npcCfg = ConfigReader.GetNpcInfo(skillID);
        if (npcCfg == null)
            return;
        SkillManagerConfig skillCfg = ConfigReader.GetSkillManagerCfg(npcCfg.NpcSkillType2);
        if (skillCfg == null)
            return;
        skillDic[player].AddSkillInfoDate(skillCfg.id, slot, npcCfg.HeadPhoto.ToString(), skillCfg.coolDown / 1000, 0, true);
    }

	//skill level up 
    public int slot
    {
        private set;
        get;
    }
    public int skillID
    {
        private set;
        get;
    }
    //void GetSkillInfo()
    //{
    //    skillID = PlayerSkillData.Instance.SkillId;
    //    slot = PlayerSkillData.Instance.Slot;
    //}
	public void SkillUpgrade(Iplayer player)
	{
        int skillID = PlayerSkillData.Instance.SkillId;
        float lastTime = PlayerSkillData.Instance.TotalTime;
		if (!skillDic.ContainsKey(player))
			return;
		
		if (slot == 5 || slot == 6) 
		{
			--slot;
		}

		SkillManagerConfig skillCfg = ConfigReader.GetSkillManagerCfg(skillID);
		if (skillCfg == null)
			return;

		skillDic[player].SkillUpgrade (slot, skillID, skillCfg.coolDown/1000);
		
	}


    private float getSkillCoolDownTime(int skillID)
    {
        if (ConfigReader.GetSkillManagerCfg(skillID) == null)
        {
            return 10.0f;
        }
        return ConfigReader.GetSkillManagerCfg(skillID).coolDown / 1000.0f;
    }

    public void ChangeSkillCd(Iplayer player)
    {
        int skillID = PlayerSkillData.Instance.SkillId;
        float lastTime = PlayerSkillData.Instance.TotalTime;
        if (!skillDic.ContainsKey(player))
            return;

        skillDic[player].ChangeColDownTime(skillID, lastTime);

        if (UIViewerPersonInfo.Instance.SetCurrClickPlayer == player)
        {
            InfoPlayerSkill(player);
        }
    }


    
	public void InfoPlayerSkill(Iplayer player)
    {
        if (!skillDic.ContainsKey(player))
            return;

		for (int i = 0; i < skillSlots.Count; i++)
		{
			skillSlots[i].Reset();

            SkillInfo skillInfo = skillDic[player].GetSkillInfo(i);

            if (skillInfo != null)
            {
                skillSlots[i].SetShowSkill(skillInfo);
            }
		}
    }

    private float startTime = 0f;
    private int curSlot = -1;
    private const float timeLimit = 0.5f;

    void OnPress(int ie, bool isPress)
    {
        if (isPress)
        {
            startTime = Time.time;
            curSlot = ie;
            return;
        }
        Iplayer player = UIViewerPersonInfo.Instance.SetCurrClickPlayer;
        curSlot = -1;
        startTime = 0f;

        EventCenter.Broadcast<bool, SkillType, Iplayer>(EGameEvent.eGameEvent_SkillDescribleType, false, SkillType.SKILL_NULL, player);
    }

    // Update is called once per frame
    void Update()
    {
        foreach (SkillInfoDate pd in skillDic.Values)
        {
            pd.Update(Time.deltaTime);
        }

        if (startTime == 0 || curSlot == -1)
            return;

        Iplayer player = UIViewerPersonInfo.Instance.SetCurrClickPlayer;
        if(!skillDic.ContainsKey(player))
            return;

        if (Time.time - startTime >= timeLimit)
        {
            EventCenter.Broadcast<bool, int, Iplayer>(EGameEvent.eGameEvent_SkillDescribleId, true, skillDic[player].GetSkillId(curSlot), player);

            startTime = 0f;
            curSlot = -1;
        }
    }

}
