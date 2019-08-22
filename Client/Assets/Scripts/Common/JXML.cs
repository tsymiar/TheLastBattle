using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace BlGame
{
    public class JString
    {
        string value;
        public JString(string str)
        {
            value = str;
        }

        public new string ToString()
        {
            return value;
        }

        //实现隐式转换 -string
        public static implicit operator string(JString xs)
        {
            return xs.value;
        }

        public static implicit operator JString(string str)
        {
            return new JString(str);
        }

        //实现隐式转换 - float
        public static implicit operator float(JString xs)
        {
            return xs.value == "" ? 0 : (float)Convert.ToDouble(xs.value);
        }
        public static implicit operator JString(float v)
        {
            return new JString(v.ToString());
        }

        //实现隐式转换 - int
        public static implicit operator int(JString xs)
        {
            return xs.value == "" ? 0 : (int)Convert.ToDouble(xs.value);
        }
        public static implicit operator JString(int v)
        {
            return new JString(v.ToString());
        }

        //实现隐式转换 - int64
        public static implicit operator JString(Int64 v)
        {
            return new JString(v.ToString());
        }
        public static implicit operator Int64(JString xs)
        {
            return xs.value == "" ? 0 : (Int64)Convert.ToDouble(xs.value);
        }

        //实现隐式转换 - bool
        public static implicit operator JString(bool v)
        {
            return new JString(v.ToString());
        }
        public static implicit operator bool(JString xs)
        {
            if (xs.value == "" || xs.value.ToLower() == "false" || xs.value == "0") return false;
            return true;
        }
    }
    public class JNode
    {
        JNode parent = null;
        string nodeName;
        protected XmlNode node = null;
        public JAttribute Attributes;
        public JNode()
        {
            Attributes = new JAttribute(this);
        }
        public JNode(JNode _parent, string _nodeName)
        {
            parent = _parent;
            nodeName = _nodeName;
            node = parent.GetChildXmlNode(nodeName);
            Attributes = new JAttribute(this);
        }
        public JNode(JNode _parent, int _index)
        {
            parent = _parent;
            node = parent.GetChildXmlNode(_index);
            Attributes = new JAttribute(this);
        }
        /// <summary>
        /// 返回该JNode对应的XmlNode
        /// </summary>
        /// <param name="Create"></param>
        /// <returns></returns>
        public XmlNode GetXmlNode(bool Create = false)
        {
            if (Create && node == null)
            {
                XmlNode xn = parent.GetXmlNode(true);
                node = xn.AppendChild(xn.OwnerDocument.CreateNode(XmlNodeType.Element, nodeName, null));
            }

            return node;
        }
        XmlNode GetChildXmlNode(int _index)
        {
            if (node == null) return null;
            return node.ChildNodes[_index];
        }
        XmlNode GetChildXmlNode(string name)
        {
            if (node == null) return null;
            return node.SelectSingleNode(name);
        }
        /// <summary>
        /// 返回子节点的个数
        /// </summary>
        public int Count
        {
            get { return node == null ? 0 : node.ChildNodes.Count; }
        }
        public string InnerXml
        {
            get { return node == null ? "" : node.InnerXml; }
            set { GetXmlNode(true).InnerXml = value; }
        }

        public JNode this[string NodeName]
        {
            get { return new JNode(this, NodeName); }
        }

        public JNode this[int NodeIndex]
        {
            get { return new JNode(this, NodeIndex); }
        }
    }

    public class JXml : JNode
    {
        XmlDocument xml;
        public JXml(string xmlstring, bool hasDocumentElement = true)
            : base()
        {
            xml = new XmlDocument();
            xml.LoadXml(xmlstring);
            node = hasDocumentElement ? (XmlNode)xml.DocumentElement : xml;
        }
        public void SaveToFile(string fileName)
        {
            System.IO.File.WriteAllText(fileName, xml.OuterXml);
        }
        public void LoadFromFile(string fileName, bool hasDocumentElement = true)
        {
            xml.Load(fileName);
            node = hasDocumentElement ? (XmlNode)xml.DocumentElement : xml;
        }
    }

    public class JAttribute
    {
        JNode node;
        public JAttribute(JNode xn)
        {
            node = xn;
        }

        public int Count
        {
            get { return node.Attributes.Count; }
        }

        void SetAttribute(string name, string value)
        {
            XmlNode xn = node.GetXmlNode(true);
            XmlAttribute xa = xn.Attributes[name];
            if (xa == null) xa = xn.Attributes.Append(xn.OwnerDocument.CreateAttribute(name));
            xa.Value = value;
        }
        public JString this[string AttributeName]
        {
            get { return (node.GetXmlNode(false) == null || node.GetXmlNode(false).Attributes[AttributeName] == null) ? "" : node.GetXmlNode(false).Attributes[AttributeName].Value; }
            set { SetAttribute(AttributeName, value); }
        }

        public XmlAttribute this[int AttributeIndex]
        {
            get { return node.GetXmlNode(false).Attributes[AttributeIndex]; }
        }
    }
}