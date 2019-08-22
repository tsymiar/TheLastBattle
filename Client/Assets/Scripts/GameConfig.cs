using System;
using System.IO;
using System.Xml;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Object = UnityEngine.Object;

namespace BlGame
{
    //游戏配置类
    public class GameConfig : Singleton<GameConfig>
    {
        //配置xml信息
        private JXml xml = null;
        
        //游戏配置文件路径 
        private string mGameConfigPath = Application.persistentDataPath + "/Config.xml";       
   
        //加载配置信息
        public void Init()
        {
            xml = new JXml("<Config></Config>");

            FileInfo t = new FileInfo(mGameConfigPath);

            //如果此文件不存在则创建
            if (!t.Exists)
            {                
                //设置特效等级
                WriteProfileString("Effect", "LodLevel", "High");
                
                xml.SaveToFile(mGameConfigPath);

              
                Debugger.Log("GameConfig not exsited");
            }
            else
            {
                //如果此文件存在则打开
                xml.LoadFromFile(mGameConfigPath);
                //Debugger.Log("");
           
                Debugger.Log("Load GameConfig not exsited");
            }  
            
            //初始配置信息
            string effectLevel = LoadProfileString("Effect", "LodLevel");

            Debugger.Log("effectLevel:" + effectLevel);

            //特效高效果
            if (effectLevel == "High")
            {
                BlGame.Effect.EffectManager.Instance.SetEffectLodLevel(Effect.EffectLodLevel.High);
            }
            //特效低效果
            else
            {
                BlGame.Effect.EffectManager.Instance.SetEffectLodLevel(Effect.EffectLodLevel.Low);
            } 
        }

        //保存配置信息
        public void Save()
        {
            xml.SaveToFile(mGameConfigPath);
        }

        //读取配置信息
        public string LoadProfileString(string section, string item)
        {
            return xml[section].Attributes[item];
        }

        //写入配置信息
        public void WriteProfileString(string section, string item, string value)
        {
            xml[section].Attributes[item] = value;            
        }      
 
        //调试配置信息
        public void Dump()
        {
            Debugger.Log("<Effect>");
            Debugger.Log("      LodLevel:" + xml["Effect"].Attributes["LodLevel"]);                      
            Debugger.Log("</Effect>");
        }
                  
    }
}