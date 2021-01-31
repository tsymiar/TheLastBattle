using System;
using UnityEngine;
using System.Xml;
using System.Collections.Generic;
using GameDefine;
using BlGame.Resource;
using System.Linq;
public class ReadHeroConfig  {

	XmlDocument xmlDoc = null;
	
	public ReadHeroConfig(string xmlFilePath)
	{

		//TextAsset xmlfile = Resources.Load(xmlFilePath) as TextAsset;
        ResourceUnit xmlfileUnit = ResourcesManager.Instance.loadImmediate(xmlFilePath, ResourceType.ASSET);
        TextAsset xmlfile = xmlfileUnit.Asset as TextAsset;

		if(!xmlfile)
		{
			//Debug.LogError(" error infos: 没有找到指定的xml文件："+xmlFilePath);
		}		

		xmlDoc = new XmlDocument();
		xmlDoc.LoadXml(xmlfile.text);
		 
		XmlNodeList infoNodeList = xmlDoc.SelectSingleNode("herocfg ").ChildNodes;
		
		for(int i = 0;i < infoNodeList.Count;i++)//XmlNode xNode in infoNodeList)
		{
			if((infoNodeList[i] as XmlElement).GetAttributeNode("un32ID") == null)	continue;
			
			string typeName = (infoNodeList[i] as XmlElement).GetAttributeNode("un32ID").InnerText;
			
			HeroConfigInfo HeroSelectInfo = new HeroConfigInfo();
			HeroSelectInfo.HeroName = Convert.ToString(typeName);
			foreach(XmlElement xEle in infoNodeList[i].ChildNodes)
			{				
				#region 搜索
				switch(xEle.Name)
				{
				case "szNOStr":	
					HeroSelectInfo.HeroNum = Convert.ToInt32(xEle.InnerText);
					break;					
				case "eMagicCate":	
					HeroSelectInfo.HeroMgicCate = Convert.ToInt32(xEle.InnerText);
					break;
				case "XuetiaoHeight": 
					HeroSelectInfo.HeroXueTiaoHeight = Convert.ToInt32(xEle.InnerText);
					break;
				case "n32AttackDist":
					HeroSelectInfo.HeroAtkDis = Convert.ToSingle(xEle.InnerText);
					break;
				case "n32BaseExp":
					HeroSelectInfo.HeroBaseExp = Convert.ToSingle(xEle.InnerText);   
					break;
				case "n32BasePhyAttPower":
					HeroSelectInfo.HeroPhyAtt = Convert.ToSingle(xEle.InnerText);
					break;	
				case "n32BaseMagAttPower":
					HeroSelectInfo.HeroMagAtt =  Convert.ToSingle(xEle.InnerText);
					break;
				case "n32BasePhyDef":
					HeroSelectInfo.HeroPhyDef =  Convert.ToSingle(xEle.InnerText); 
					break;
				case "n32BaseMagDef":
					HeroSelectInfo.HeroMagDef =  Convert.ToSingle(xEle.InnerText); 
					break;		
				case "n32BaseMoveSpeed":
					HeroSelectInfo.HeroMoveSpeed =  Convert.ToSingle(xEle.InnerText); 
					break;		
                 case "n32BaseMoveSpeedScaling":
                    HeroSelectInfo.n32BaseMoveSpeedScaling = Convert.ToSingle(xEle.InnerText);
                    break;
				case "n32BaseAttackCD":
					HeroSelectInfo.HeorBaseAtkCd =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32BaseMaxHP":
					HeroSelectInfo.HeroMaxHp =  Convert.ToSingle(xEle.InnerText); 
					break;		
				case "n32BaseMaxMP":
					HeroSelectInfo.HeroMaxMp =  Convert.ToSingle(xEle.InnerText); 
					break;		
				case "n32BaseHPRecover":
					HeroSelectInfo.HeroHpRecover =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32BaseMPRecover":
					HeroSelectInfo.HeroMpRecover =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32BaseReliveTime":
					HeroSelectInfo.HeroRelieveTime =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32ExpGrowth":
					HeroSelectInfo.HeroExpGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32PhyAttGrowth":
					HeroSelectInfo.HeroPhyAttGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32MagAttGrowth":
					HeroSelectInfo.HeroMagAttGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32PhyDefGrowth":
					HeroSelectInfo.HeroPhyDefGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32MagDefGrowth":
					HeroSelectInfo.HeroMagDefGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32AttackCDGrowth":
					HeroSelectInfo.HeroAtkCdGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "32MaxHPGrowth":
					HeroSelectInfo.HeroMaxHpGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "32MaxMPGrowth":
					HeroSelectInfo.HeroMaxMpGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32HPRecoverGrowth":
					HeroSelectInfo.HeroHpRecoverGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32MPRecoverGrowth":
					HeroSelectInfo.HeroHpRecoverGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32ReliveGrowth":
					HeroSelectInfo.HeroHpRecoverGrowth =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32CPRecover":
					HeroSelectInfo.HeroCpRecover =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "n32CollideRadious":
					HeroSelectInfo.HeroCollideRadious =  Convert.ToSingle(xEle.InnerText); 
					break;	
				case "un32DeathSould":
					HeroSelectInfo.HeroDeathSould =  Convert.ToString(xEle.InnerText); 
					break;
                case "un32WalkSound":
                    HeroSelectInfo.un32WalkSound = xEle.InnerText; 
                    break;
				case "un32Script1":
					HeroSelectInfo.HeroScript1 =  Convert.ToString(xEle.InnerText); 
					break;	
				case "n32Script1Rate":
                    HeroSelectInfo.HeroScript1Rate = Convert.ToString(xEle.InnerText); 
					break;
				case "un32SkillType1":
					HeroSelectInfo.HeroSkillType1 =  Convert.ToInt32(xEle.InnerText); 
					break;
				case "un32SkillType2":
					HeroSelectInfo.HeroSkillType2 =  Convert.ToInt32(xEle.InnerText); 
					break;
				case "un32SkillType3":
					HeroSelectInfo.HeroSkillType3 =  Convert.ToInt32(xEle.InnerText); 
					break;
				case "un32SkillType4":
					HeroSelectInfo.HeroSkillType4 =  Convert.ToInt32(xEle.InnerText); 
					break;
				case "un32SkillType5":
					HeroSelectInfo.HeroSkillType5 =  Convert.ToInt32(xEle.InnerText); 
					break;
                case "un32SkillType6":
                    HeroSelectInfo.HeroSkillType6 = Convert.ToInt32(xEle.InnerText);
                    break;
                case "n32LockRadious":
                    HeroSelectInfo.n32LockRadious = Convert.ToSingle(xEle.InnerText); 
                    break;
                case "n32RandomAttack":
                    HeroSelectInfo.n32RandomAttack = GameDefine.GameMethod.ResolveToStrList(xEle.InnerText);
                    break;
                case "un32PreItem":
                    {
                        string str = Convert.ToString(xEle.InnerText);
                        if (str.CompareTo("0") != 0)
                        {
                            HeroSelectInfo.HeroPreEquip = GameMethod.ResolveToIntList(str, ',');
                        }
                    }
                    break;
                case "un32MidItem":
                    {
                        string str = Convert.ToString(xEle.InnerText);
                        if (str.CompareTo("0") != 0)
                        {
                            HeroSelectInfo.HeroMidEquip = GameMethod.ResolveToIntList(str, ',');
                        }
                    }
                    break;
                case "un32ProItem":
                    {
                        string str = Convert.ToString(xEle.InnerText);
                        if (str.CompareTo("0") != 0)
                        {
                            HeroSelectInfo.HeroLatEquip = GameMethod.ResolveToIntList(str, ',');
                        }
                    }
                    break;
                case "HeroKind":
                    {
                        string str = Convert.ToString(xEle.InnerText); 
                        List<int> list = GameMethod.ResolveToIntList(str, ',');
                        
                        for (int j = 0; j < list.Count; j++)
                        {
                            HeroSelectInfo.heroKind.Add((HeroType)list.ElementAt(j));
                        } 
                    }
                    break;
                    case "un32SkillTypeP":
                         HeroSelectInfo.HeroSkillTypeP = Convert.ToInt32(xEle.InnerText);
                    break;
				}
				#endregion
			}		
			ConfigReader.heroXmlInfoDict.Add(HeroSelectInfo.HeroNum,HeroSelectInfo);
		}
	}
}

public class HeroConfigInfo:System.Object
{
	#region 道具信息
	public string HeroName;
	public int    HeroNum;       
	public int    HeroMgicCate;   
	public float  HeroXueTiaoHeight;
	public float  HeroAtkDis;
	public float  HeroBaseExp;
	public float  HeroPhyAtt;
	public float  HeroMagAtt;
	public float  HeroPhyDef;
	public float  HeroMagDef;
	public float  HeroMoveSpeed;
    public float n32BaseMoveSpeedScaling;
	public float  HeorBaseAtkCd;
	public float  HeroMaxHp;
	public float  HeroMaxMp;
	public float  HeroHpRecover;
	public float  HeroMpRecover;
	public float  HeroRelieveTime;
	public float  HeroExpGrowth;
	public float  HeroPhyAttGrowth;
	public float  HeroMagAttGrowth;
	public float  HeroPhyDefGrowth;
	public float  HeroMagDefGrowth;
	public float  HeroAtkCdGrowth;
	public float  HeroMaxHpGrowth;
	public float  HeroMaxMpGrowth;
	public float  HeroHpRecoverGrowth;
	public float  HeroMpRecoverGrowth;
	public float  HeroReliveGrowth;
	public float  HeroCpRecover;
	public float  HeroCollideRadious;
    public float n32LockRadious;
    public string un32WalkSound;
	public string HeroDeathSould;
	public string HeroScript1;
    public string HeroScript1Rate;
	public int    HeroSkillType1;
	public int    HeroSkillType2;
	public int	  HeroSkillType3;
	public int	  HeroSkillType4;
	public int    HeroSkillType5;
    public int    HeroSkillType6;
    public int  HeroSkillTypeP;
    public List<string> n32RandomAttack;
    public List<int> HeroPreEquip = new List<int>();
    public List<int> HeroMidEquip = new List<int>();
    public List<int> HeroLatEquip = new List<int>();
    public List<HeroType> heroKind = new List<HeroType>();
	#endregion
	
}
