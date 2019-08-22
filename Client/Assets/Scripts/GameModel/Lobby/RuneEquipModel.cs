using UnityEngine;
using System.Collections.Generic;
using BlGame;
using JT.FWW.GameData;
using BlGame.GameData;
using BlGame.Network;
using LSToGC;
using System.IO;
using System.Linq;

namespace BlGame.Model
{
    class RuneEquipModel : Singleton<RuneEquipModel>
    {
        public int pagenum
        {
            set;
            get;
        }
        public RuneEquipModel()
        {
            pagenum = 0;
        }

        public int getRealPos(int page, int pos)
        {
            return page * GameDefine.GameConstDefine.MaxRuneSlot + pos;
        }

        public void UpdateRuneSlotInfo(uint runeid, int page, int pos)
        {
            if (page > pagenum)
            {
                pagenum = page;
            }
            runePgae2SlotInfoMap[getRealPos(page, pos)] = runeid;

            EventCenter.Broadcast<uint, int, int>(EGameEvent.eGameEvent_RuneQuipUpdate, runeid, page, pos);
        }

        public void RemoveRune(int page, int pos)
        {
            runePgae2SlotInfoMap.Remove(getRealPos(page, pos));
            EventCenter.Broadcast<int, int>(EGameEvent.eGameEvent_RuneQuipUnload, page, pos);
        }

        public bool IfHvRune(int page, int pos)
        {
            var realPos = getRealPos(page, pos);
            return runePgae2SlotInfoMap.ContainsKey(realPos);
        }
        public Dictionary<int, uint> GetrunePos2RuneidMap()
        {
            return runePgae2SlotInfoMap;
        }

        public void Clear()
        {
            runePgae2SlotInfoMap.Clear();
        }

        Dictionary<int, uint> runePgae2SlotInfoMap = new Dictionary<int, uint>();
    }
}
