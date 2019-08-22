using UnityEngine;
using System.Collections;

namespace BlGame.Ctrl
{
    public class DeathCtrl : Singleton<DeathCtrl>
    {
        public void Enter()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_NotifyDeathEnter);
        }

        public void Exit()
        {
            EventCenter.Broadcast(EGameEvent.eGameEvent_NotifyDeathExit);
        }

        public void SetTime(float start, float relieve, long money, int num, bool isV = true)
        {
            start /= 1000f;
            mbeforeTime = start;
            mLastTime = relieve;
            mRecoveryNums = num;
            mMoney = money;
        }
        public int mRecoveryNums = 0;
        public long mMoney = 0;
        public float mLastTime = 0f, mbeforeTime = 0f;


        internal void SendBtn()
        {
            CGLCtrl_GameLogic.Instance.Resurrection();
        }
    }
}