using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using BlGame;
using BlGame.GameState;

namespace BlGame.View
{
    public enum EScenesType
    {
        EST_None,
        EST_Login,
        EST_Play,
    }

    public enum EWindowType
    {
        EWT_LoginWindow,
        EWT_UserWindow,
        EWT_LobbyWindow,
        EWT_BattleWindow,
        EWT_RoomWindow,
        EWT_HeroWindow,
        EWT_BattleInfoWindow,
        EWT_MarketWindow,
        EWT_MarketHeroListWindow,
        EWT_MarketHeroInfoWindow,
        EWT_MarketRuneListWindow,
        EWT_MarketRuneInfoWindow,
        EWT_SocialWindow,
        EWT_GamePlayWindow,
        EWT_InviteWindow,
        EWT_ChatTaskWindow,
        EWT_ScoreWindow,
        EWT_InviteAddRoomWindow,
        EWT_RoomInviteWindow,
        EWT_TeamMatchWindow,
        EWT_TeamMatchInvitationWindow,
        EWT_TeamMatchSearchingWindow,
        EWT_MailWindow,
        EWT_HomePageWindow,
        EWT_PresonInfoWindow,
        EWT_ServerMatchInvitationWindow,
        EWT_SoleSoldierWindow,
        EWT_MessageWindow,
        EWT_MiniMapWindow,
        EWT_VIPPrerogativeWindow,
        EWT_RuneEquipWindow,
        EWT_DaliyBonusWindow,
        EWT_EquipmentWindow,
        EWT_SystemNoticeWindow,
        EWT_TimeDownWindow,
        EWT_RuneCombineWindow,
		EWT_HeroDatumWindow,
		EWT_RuneRefreshWindow,
        EWT_GamePlayGuideWindow,
        EMT_PurchaseSuccessWindow,
        EMT_GameSettingWindow,
        EMT_AdvancedGuideWindow,
        EMT_ExtraBonusWindow,
        EMT_EnemyWindow,
        EMT_HeroTimeLimitWindow,
        EMT_SkillWindow,
        EMT_SkillDescribleWindow,
        EMT_RuneBuyWindow,
        EMT_DeathWindow,
    }

    public class WindowManager : Singleton<WindowManager>
    {
        public WindowManager()
        {
            mWidowDic = new Dictionary<EWindowType, BaseWindow>();

            mWidowDic[EWindowType.EWT_LoginWindow] = new LoginWindow();
            mWidowDic[EWindowType.EWT_UserWindow] = new UserInfoWindow();
            mWidowDic[EWindowType.EWT_LobbyWindow] = new LobbyWindow();
            mWidowDic[EWindowType.EWT_BattleWindow] = new BattleWindow();
            mWidowDic[EWindowType.EWT_RoomWindow] = new RoomWindow();
            mWidowDic[EWindowType.EWT_HeroWindow] = new HeroWindow();
            mWidowDic[EWindowType.EWT_BattleInfoWindow] = new BattleInfoWindow();
            mWidowDic[EWindowType.EWT_MarketWindow] = new MarketWindow();
            mWidowDic[EWindowType.EWT_MarketHeroListWindow] = new MarketHeroListWindow();
            mWidowDic[EWindowType.EWT_MarketHeroInfoWindow] = new MarketHeroInfoWindow();    
            mWidowDic[EWindowType.EWT_SocialWindow] = new SocialWindow();
            mWidowDic[EWindowType.EWT_GamePlayWindow] = new GamePlayWindow();
            mWidowDic[EWindowType.EWT_InviteWindow] = new InviteWindow();
            mWidowDic[EWindowType.EWT_ChatTaskWindow] = new ChatTaskWindow();
            mWidowDic[EWindowType.EWT_ScoreWindow] = new ScoreWindow();
            mWidowDic[EWindowType.EWT_InviteAddRoomWindow] = new InviteAddRoomWindow();
            mWidowDic[EWindowType.EWT_RoomInviteWindow] = new RoomInviteWindow();
            mWidowDic[EWindowType.EWT_TeamMatchWindow] = new TeamMatchWindow();
            mWidowDic[EWindowType.EWT_TeamMatchInvitationWindow] = new TeamMatchInvitationWindow();
            mWidowDic[EWindowType.EWT_TeamMatchSearchingWindow] = new TeamMatchSearchingWindow();
            mWidowDic[EWindowType.EWT_MailWindow] = new MailWindow();
            mWidowDic[EWindowType.EWT_HomePageWindow] = new HomePageWindow(); 
            mWidowDic[EWindowType.EWT_PresonInfoWindow] = new PresonInfoWindow();
            mWidowDic[EWindowType.EWT_ServerMatchInvitationWindow] = new ServerMatchInvitationWindow();
            mWidowDic[EWindowType.EWT_SoleSoldierWindow] = new SoleSoldierWindow();
            mWidowDic[EWindowType.EWT_MessageWindow] = new MessageWindow();
            mWidowDic[EWindowType.EWT_MarketRuneListWindow] = new MarketRuneListWindow();
            mWidowDic[EWindowType.EWT_MiniMapWindow] = new MiniMapWindow();
            mWidowDic[EWindowType.EWT_MarketRuneInfoWindow] = new MarketRuneInfoWindow();
            mWidowDic[EWindowType.EWT_VIPPrerogativeWindow] = new VIPPrerogativeWindow();
            mWidowDic[EWindowType.EWT_RuneEquipWindow] = new RuneEquipWindow();
            mWidowDic[EWindowType.EWT_DaliyBonusWindow] = new DaliyBonusWindow();
            mWidowDic[EWindowType.EWT_EquipmentWindow] = new EquipmentWindow();
            mWidowDic[EWindowType.EWT_SystemNoticeWindow] = new SystemNoticeWindow();
            mWidowDic[EWindowType.EWT_TimeDownWindow] = new TimeDownWindow();
            mWidowDic[EWindowType.EWT_RuneCombineWindow] = new RuneCombineWindow();
			mWidowDic[EWindowType.EWT_HeroDatumWindow] = new HeroDatumWindow();
			mWidowDic[EWindowType.EWT_RuneRefreshWindow] = new RuneRefreshWindow();
            mWidowDic[EWindowType.EWT_GamePlayGuideWindow] = new GamePlayGuideWindow();
            mWidowDic[EWindowType.EMT_PurchaseSuccessWindow] = new PurchaseSuccessWindow();
            mWidowDic[EWindowType.EMT_GameSettingWindow] = new GameSettingWindow();
            mWidowDic[EWindowType.EMT_AdvancedGuideWindow] = new AdvancedGuideWindow();
            mWidowDic[EWindowType.EMT_ExtraBonusWindow] = new ExtraBonusWindow();
            mWidowDic[EWindowType.EMT_EnemyWindow] = new EnemyWindow();
            mWidowDic[EWindowType.EMT_HeroTimeLimitWindow] = new HeroTimeLimitWindow();
            mWidowDic[EWindowType.EMT_SkillWindow] = new SkillWindow();
            mWidowDic[EWindowType.EMT_SkillDescribleWindow] = new SkillDescribleWindow();
            mWidowDic[EWindowType.EMT_RuneBuyWindow] = new RuneBuyWindow();
            mWidowDic[EWindowType.EMT_DeathWindow] = new DeathWindow();
        }

        public BaseWindow GetWindow(EWindowType type)
        {
            if (mWidowDic.ContainsKey(type))
                return mWidowDic[type];

            return null;
        }

        public void Update(float deltaTime)
        {
            foreach (BaseWindow pWindow in mWidowDic.Values)
            {
                if (pWindow.IsVisible())
                {
                    pWindow.Update(deltaTime);
                }
            }
        }

        public void ChangeScenseToPlay(EScenesType front)
        {
            foreach (BaseWindow pWindow in mWidowDic.Values)
            {
                if (pWindow.GetScenseType() == EScenesType.EST_Play)
                {
                    pWindow.Init();

                    if(pWindow.IsResident())
                    {
                        pWindow.PreLoad();
                    }
                }
                else if ((pWindow.GetScenseType() == EScenesType.EST_Login) && (front == EScenesType.EST_Login))
                {
                    pWindow.Hide();
                    pWindow.Realse();

                    if (pWindow.IsResident())
                    {
                        pWindow.DelayDestory();
                    }
                }
            }
        }

        public void ChangeScenseToLogin(EScenesType front)
        {
            foreach (BaseWindow pWindow in mWidowDic.Values)
            {
                if (front == EScenesType.EST_None && pWindow.GetScenseType() == EScenesType.EST_None)
                {
                    pWindow.Init();

                    if (pWindow.IsResident())
                    {
                        pWindow.PreLoad();
                    }
                }

                if (pWindow.GetScenseType() == EScenesType.EST_Login)
                {
                    pWindow.Init();

                    if (pWindow.IsResident())
                    {
                        pWindow.PreLoad();
                    }
                }
                else if ((pWindow.GetScenseType() == EScenesType.EST_Play) && (front == EScenesType.EST_Play))
                {
                    pWindow.Hide();
                    pWindow.Realse();

                    if (pWindow.IsResident())
                    {
                        pWindow.DelayDestory();
                    }
                }
            }
        }

        /// <summary>
        /// 隐藏该类型的所有Window
        /// </summary>
        /// <param name="front"></param>
        public void HideAllWindow(EScenesType front)
        {
            foreach (var item in mWidowDic)
            {
                if (front == item.Value.GetScenseType())
                {
                    Debug.Log(item.Key);
                    item.Value.Hide();
                    //item.Value.Realse();
                }
            }
        }

        public void ShowWindowOfType(EWindowType type)
        { 
            BaseWindow window;
            if(!mWidowDic.TryGetValue(type , out window))
            {
                return;
            }
            window.Show();
        }

        private Dictionary<EWindowType, BaseWindow> mWidowDic;
    }


}
