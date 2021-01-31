using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using UnityEngine;
using System;
using BlGame.Ctrl;

using GCToSS;

namespace BlGame.Model
{
    public class UIGuideModel : Singleton<UIGuideModel>
    {

        /// <summary>
        /// 获取任务模块要完成的任务id列表
        /// </summary>
        /// <param name="mdId"></param>
        /// <returns></returns>
        public List<int> GetTaskIdList(int mdId)
        {
            List<int> taskList = new List<int>();
            foreach (var item in ConfigReader.GuideTaskXmlInfoDict)
            {
                if (item.Value.ModelID == mdId)
                {
                    taskList.Add(item.Key);
                    continue;
                }
            }
            return taskList;
        }

        /// <summary>
        /// 引导类型
        /// </summary>
        public GCToCS.AskCSCreateGuideBattle.guidetype GuideType = GCToCS.AskCSCreateGuideBattle.guidetype.other;

        public mGuideStepInfo mCurrentTaskModelId
        {
            private set;
            get;
        }

        /// <summary>
        /// 引导已完成的模块list
        /// </summary>
        /// <param name="pList"></param>
        public void GuideFinishModelList(List<uint> pList)
        {
            mGuideFinishList = pList;
            mCurrentTaskModelId  = mGuideStepInfo.GuideStepNull;
            foreach (mGuideStepInfo step in Enum.GetValues(typeof(mGuideStepInfo)))
            {
                if (step == mGuideStepInfo.GuideStepNull)
                {
                    continue;
                }
                if (!mGuideFinishList.Contains((uint)step))
                {
                    mCurrentTaskModelId = step;
                    break;
                }
            }
            if (mCurrentTaskModelId != mGuideStepInfo.GuideStepNull)
            {
                GamePlayGuideCtrl.Instance.StartModelTask(mCurrentTaskModelId);
                UIGuideCtrl.Instance.Enter();
            }
        }

        /// <summary>
        /// 清理数据
        /// </summary>
        public void ClearModelData()
        {
            mGuideFinishList.Clear();
            UiGuideButtonGameObjectList.Clear();
        }

        /// <summary>
        /// 获取要复制的按钮
        /// </summary>
        /// <param name="ne"></param>
        /// <returns></returns>
        public GameObject GetUiGuideButtonGameObject(string ne)
        {
            foreach (GameObject item in UiGuideButtonGameObjectList)
            {
                if (item == null)
                {
                    continue;
                }
                if (item.name == ne)
                {
                    return item;
                }
            }
            return null;
        }
        public List<GameObject> UiGuideButtonGameObjectList = new List<GameObject>();

        /// <summary>
        /// 已完成模块ID列表
        /// </summary>
        public List<uint> mGuideFinishList = new List<uint>();

        public bool mIsGuideComp;
    }

    /// <summary>
    /// 新手引导总的分四个大步骤
    /// </summary>
    public enum mGuideStepInfo
    {
        GuideStepNull = 0,
        PrimaryGuide = 1001,
        BuyHeroGuide,
        BattleAiGuide,
        BuyRuneGuide,
    }
}