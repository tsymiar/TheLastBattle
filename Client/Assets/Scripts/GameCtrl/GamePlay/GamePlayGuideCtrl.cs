using UnityEngine;
using System.Collections;
using BlGame.Model;

namespace BlGame.Ctrl
{
    public class GamePlayGuideCtrl : Singleton<GamePlayGuideCtrl>
    {
        public GamePlayGuideCtrl()
        {

        }

        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_PlayGuideEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_PlayGuideExit);
        }

        ////////////////////////////继承接口/////////////////////////
        //类对象初始化

        /// <summary>
        /// 向服务器发送要执行的任务Id
        /// </summary>
        public void AskSSGuideStepComp(GCToSS.AskSSGuideStepComp.edotype ebornsolder, int mTaskId)
        {
            CGLCtrl_GameLogic.Instance.AskSSGuideStepComp(ebornsolder, mTaskId);
        }

        /// <summary>
        /// 开始执行模块的引导
        /// </summary>
        /// <param name="modelId"></param>
        public void StartModelTask(mGuideStepInfo modelId)
        {
            GamePlayGuideModel.Instance.StartModelTask(modelId);
        }

        /// <summary>
        /// 完成模块Id
        /// </summary>
        /// <param name="mdId"></param>
        /// <param name="comp"></param>
        public void GuideCSStepComplete(int mdId , bool allComp)
        {
            CGLCtrl_GameLogic.Instance.GuideCSStepComplete(mdId, allComp);
            if (allComp)
            {
                UIGuideModel.Instance.mIsGuideComp = allComp;
                UIGuideModel.Instance.ClearModelData();
                GamePlayGuideModel.Instance.ClearModelData();
            }
        }

    }
}

