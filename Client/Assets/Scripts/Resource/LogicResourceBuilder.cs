using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
namespace BlGame.Resource
{
    public delegate void OnLogicResourceBuildedstring(string path);
    public class LogicResourceBuilder
    {
        public string resPath;
        public List<string> resLists = new List<string>();
        public bool isDown;
        public LogicResouce.ELogicResType logicResType;
        public OnLogicResourceBuildedstring onLogicResourceBuilded;
    }
}
