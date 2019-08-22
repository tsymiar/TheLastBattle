using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

namespace BlGame
{
    public class CEvent
    {
        private EGameEvent eventId;
        private Dictionary<string, object> paramList;

        public CEvent()
        {
            paramList = new Dictionary<string, object>();
        }

        public CEvent(EGameEvent id)
        {
            eventId = id;
            paramList = new Dictionary<string, object>();
        }

        public EGameEvent GetEventId()
        {
            return eventId;
        }

        public void AddParam(string name, object value)
        {
            paramList[name] = value;
        }

        public object GetParam(string name)
        {
            if (paramList.ContainsKey(name))
            {
                return paramList[name];
            }
            return null;
        }

        public bool HasParam(string name)
        {
            if (paramList.ContainsKey(name))
            {
                return true;
            }
            return false;
        }

        public int GetParamCount()
        {
            return paramList.Count;
        }

        public Dictionary<string, object> GetParamList()
        {
            return paramList;
        }
    }
}