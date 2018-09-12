using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace RemoteConsole
{
    public partial class Form1 : Form
    {
        public CenterServer.notice_info notice = new CenterServer.notice_info();
        public List<string> str_list = new List<string>();
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Text = "预览";
            Decode_noticeinfo(notice.notice);
            //richTextBox1.Text = notice.notice;
            button1.Text = notice.titel_str;
            if (notice.flag == CenterServer.EFlag.eFlag_None)
            {
                button2.Visible = false;
            }

            switch(notice.flag){
                case CenterServer.EFlag.eFlag_Notice:
                    button2.Text = "通知";
                    break;
                case CenterServer.EFlag.eFlag_OnSale:
                    button2.Text = "促销";
                    break;
                case CenterServer.EFlag.eFlag_Services:
                    button2.Text = "活动";
                    break;
                default:
                    break;
            }

            if (notice.state == CenterServer.EState.eState_None)
            {
                button3.Visible = false;
            }

            switch (notice.state)
            {
                case CenterServer.EState.eState_New:
                    button3.Text = "新！";
                    break;
                case CenterServer.EState.eState_Hot:
                    button3.Text = "火！";
                    break;
                default:
                    break;
            }
            /*Decode_string(notice.notice);*/
        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            //Decode_string(richTextBox1.Text.ToString());
        }

        private void Decode_noticeinfo(string src)
        {
           
            List<string> list = new List<string>();
            string[] sArray = src.Split(new char[3] { '[', ']','-'});
            foreach (string i in sArray)
            {
                if (i.ToString().Length > 0)
                {
                    list.Add(i.ToString());
                }
            }

            for (int i = 0; i < list.Count; i ++)
            {
                if (i % 2 != 0)
                {
                    richTextBox1.Text += list[i];
                }
            }

            Color color = Color.Black;

            int j = 0;
            int k = 0;
            int count = 0;
            for(int i = 0; i < list.Count; i++)
            {
                if (i  == 0 || i % 2 == 0)
                {
                    color = System.Drawing.ColorTranslator.FromHtml("#" + list[i]);
                }
                else
                {
                    j += list[i].Length;
                    for (; k < j;  k++)
                    {
                        richTextBox1.Select(k, 1);
                        richTextBox1.SelectionColor = color;
                    }

                }
            }
        }

        private void Decode_title(string src)
        {
            List<string> list = new List<string>();
            string[] sArray = src.Split(new char[3] { '[', ']', '-' });
            foreach (string i in sArray)
            {
                if (i.ToString().Length > 0)
                {
                    list.Add(i.ToString());
                }
            }

            for (int i = 0; i < list.Count; i++)
            {
                if (i % 2 != 0)
                {
                    button1.Text += list[i];
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //this.Name = notice.titel_str;
        }

        private void label2_Click(object sender, EventArgs e)
        {
           // this.Name = notice.flag.ToString();
        }

        private void label1_Click(object sender, EventArgs e)
        {
           // this.Name = notice.state.ToString();
        }

        private void button3_Click(object sender, EventArgs e)
        {

        }
    }
}
