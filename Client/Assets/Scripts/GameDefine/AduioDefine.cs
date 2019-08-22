using UnityEngine;
using System.Collections;

namespace GameDefine
{
    public static class AudioDefine
    { 
        public static string PATH_UIBGSOUND = "Audio/EnvironAudio/mus_fb_login_lp";
        public static string PATH_ENVIRONAUDIO = "Audio/EnvironAudio/";

        public static string PATH_GETMONEY = "Audio/UIAudio/Tjinbi";

        public static string PATH_UnloadRune = "Audio/UIAudio/Dfasong";

        public static string PATH_ENTERALTAR_SOUND = "Audio/UIAudio/Tjitanjianzao";
        public static string PATH_LEVELUP_SOUND = "Audio/UIAudio/Tshengji";
        public static string PATH_REBORN_SOUND = "Audio/UIAudio/Tfuhui";
        public static string PATH_CLICKSHOP_SOUND = "Audio/HeroSelect/vo_merchant";
        public static string ITEMCOMBINESOUNDEFFECT = "Audio/UIAudio/Thecheng";

        public static string PATH_HERO_KILLSOUND = "Audio/AudioKill/";
        public static string PATH_HERO_DEADSOUND = "Audio/sounddead/";

        public static string PATH_HERO_SELECT_COUNTDOWN = "Audio/UIAudio/daojishi"; //英雄选择倒计时
        public static string PATH_JUNGLE_MONSTER_BE_ATK_SOUND = "Audio/HeroSelect/";


        public static string PATH_GAME_OVER_FAILE = "Audio/EnvironAudio/GameOverVictory";
        public static string PATH_GAME_OVER_VICTORY = "Audio/EnvironAudio/GameOverVictory";

        public static string PATH_CITY_AUDIO = "sound/huicheng";

        /// <summary>
        /// 行走音效
        /// </summary>
        public static string PATH_HERO_WALKSOUND = "Audio/soundwalk/";

        public static string GetMapBgAudio(MAPTYPE type)
        {
            string mapName = "";
            switch (type)
            {
                case MAPTYPE.EASYMAP:
                    mapName = "Audio/EnvironAudio/mus_fb_PVP004_lp_primarybattlefield";
                    //mapName = MiddleMapName;
                    break;
                case MAPTYPE.NEWS_GUIDE_1:
                case MAPTYPE.MIDDLEMAP:
                    mapName = "Audio/EnvironAudio/mus_fb_PVP002_lp_intermediatebattlefield";
                    break;
                case MAPTYPE.HIGHMAP:
                    mapName = "Audio/EnvironAudio/mus_fb_PVP001_lp_seniorbattlefield";
                    break;
            }
            return mapName;
        }
    }
}