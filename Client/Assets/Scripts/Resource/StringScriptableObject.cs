using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif 

namespace BlGame.Resource
{
    public class StringScriptableObject:ScriptableObject
    {
        public string[] content;
    }
}
