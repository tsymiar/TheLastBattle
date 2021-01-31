using UnityEngine;
using UnityEditor;
using System.IO;
using System.Collections;


    public class BuildMethod
    {
        //public static string getNameByPath(string path)
        //{
        //    String[] words = path.Split(new char[] { '/','\\' });
        //    if (words != null && words.Length != 0)
        //    {
        //        String[] splitstr = words[words.Length - 1].Split(new char[] { '.' });
        //        if (splitstr != null && splitstr.Length != 0)
        //        {
        //            String name = splitstr[0];
        //            return name;
        //        }
        //    }
        //    return "";
        //}
        public static string getUnityPathInResourceFolder(string path)
        {
            string[] rootwords = Application.dataPath.Split(new char[] { '/', '\\' });
            int count = rootwords.Length;
            string[] words = path.Split(new char[] { '/', '\\' });
            if (words != null)
            {
                string rtvalue = words[count + 2];
                for (int i = count + 3; i < words.Length; ++i)
                {

                    rtvalue += ("/" + words[i]);
                }
                return rtvalue;
            }
            return "";
        }
        public static string getUnityPath(string path)
        {
            string[] rootwords = Application.dataPath.Split(new char[] { '/', '\\' });
            int count = rootwords.Length;
            string[] words = path.Split(new char[] { '/', '\\' });
            if (words != null)
            {
                string rtvalue = words[count - 1];
                for (int i = count; i < words.Length; ++i)
                {

                    rtvalue += ("/" + words[i]);
                }
                return rtvalue;
            }
            return "";
        }
        public static string getShaderName(string sName)
        {
            //for (int i = 0; i < sName.Length; ++i)
            //{
            //    if (sName[i] == '/' && sName[i] == '\\')
            //    {
            //        sName. = '_';
            //    }
            //}
            string[] words = sName.Split(new char[] {' ','/', '\\' });
            string retstr = words[0];
            for (int i = 1; i < words.Length; ++i)
            {
                retstr += ('_' + words[i]);
            }
            return retstr;
        }
    }

