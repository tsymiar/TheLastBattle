using System;
using System.IO;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BlGame;

namespace BlGame.Resource
{
    class ArchiveManager : Singleton<ArchiveManager>
    {
        internal Dictionary<string, Archive> mAllArchives;

        public ArchiveManager()
        {
            mAllArchives = new Dictionary<string, Archive>();
        }

        public void Init()
        {
            StreamReader sr = ResourcesManager.OpenText("Resource");
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(sr.ReadToEnd());
            XmlElement root = doc.DocumentElement;
            IEnumerator iter = root.GetEnumerator();
            while (iter.MoveNext())
            {
                XmlElement child_root = iter.Current as XmlElement;
                IEnumerator child_iter = child_root.GetEnumerator();
                if (!mAllArchives.ContainsKey(child_root.Name))
                {
                    Archive arh = new Archive();
                    mAllArchives.Add(child_root.Name, arh);
                }
                while (child_iter.MoveNext())
                {
                    XmlElement file = child_iter.Current as XmlElement;
                    string name = file.GetAttribute("name");
                    string type = file.GetAttribute("type");
                    mAllArchives[child_root.Name].add(name, type);
                }
            }
            sr.Close();
        }

        public string getPath(string archiveName, string fileName)
        {
            if (mAllArchives.ContainsKey(archiveName))
                return mAllArchives[archiveName].getPath(fileName);
            else
                DebugEx.LogError("can not find " + archiveName, ResourceCommon.DEBUGTYPENAME);
            return null;
        }

        //public void dumpAllArchives()
        //{
        //    foreach (KeyValuePair<string, Archive> a in mAllArchives)
        //    {
        //        Debug.Log(" the archive is : " + a.Key);
        //        a.Value.dumpAllFiles();
        //    }
        //}
    }
}
   
