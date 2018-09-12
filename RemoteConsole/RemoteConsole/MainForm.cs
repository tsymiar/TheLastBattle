using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Printing;
using MySql.Data.MySqlClient;
using System.IO;

namespace RemoteConsole
{
    public partial class MainForm : Form
    {
        public CenterServer nowSelectServer = new CenterServer();

        public Channel nowSdk;

        private TextBox[] CDKeyItemText = new TextBox[5];
        private TextBox[] CDKeyItemNumText = new TextBox[5];

        private int CDKeyItem = 0;
        private string connectionString = "";
        private int search_event_id = 0;
        private int ShowCDKeyInfoLine = 30;
        private int ShowCdkeyInfoPage = 1;
        private int ShowCdkeyInfoTotalPage = 1;
        private int ShowCDKeyEventLine = 30;
        private int ShowCdkeyEventPage = 1;
        private int ShowCdkeyEventTotalPage = 1;

        public CenterServer.notice_info notice = new CenterServer.notice_info();
        public CenterServer.CDKey_Event CDKeyInfo = new CenterServer.CDKey_Event();

        public List<CenterServer.SCDKey_Event> CDkey_Events = new List<CenterServer.SCDKey_Event>();
        public List<CenterServer.SCDKey_Info> CDkey_Infos = new List<CenterServer.SCDKey_Info>();

        public MainForm()
        {
            InitializeComponent();

            CSServerView.Columns.Add("名称", 100);
            CSServerView.Columns.Add("IP", 100);
            CSServerView.Columns.Add("状态", 80);

            for (int i = 0; i < 5; i++)
            {
                CDKeyItemText[i] = new TextBox();
                System.Drawing.Point p = new Point(243, 476 + i * 30);//创建一个坐标,用来给新的输入框定位
                CDKeyItemText[i].Location = p;
                CDKeyItemText[i].BackColor = Color.Silver;
                tabPage6.Controls.Add(CDKeyItemText[i]);//向tabPage6中添加此按钮

                CDKeyItemNumText[i] = new TextBox();
                System.Drawing.Point p1 = new Point(451, 476 + i * 30);
                CDKeyItemNumText[i].Location = p1;
                CDKeyItemNumText[i].BackColor = Color.Silver;
                tabPage6.Controls.Add(CDKeyItemNumText[i]);
                if (i != 0)
                {
                    CDKeyItemNumText[i].Visible = false;
                    CDKeyItemText[i].Visible = false;
                }
            }
            CDKeyItem = 1;

            listView_CDkey.Columns.Add("标题", 200);
            listView_CDkey.Columns.Add("平台", 100);
            listView_CDkey.Columns.Add("过期时间", 150);
            listView_CDkey.Columns.Add("code个数", 90);
            listView_CDkey.Columns.Add("code长度", 95);
            listView_CDkey.Columns.Add("类型", 80);

            CDKeyView.Columns.Add("兑换码", 200);
            CDKeyView.Columns.Add("状态", 150);
            CDKeyView.Columns.Add("领取时间", 150);
            CDKeyView.Columns.Add("区服", 100);
            CDKeyView.Columns.Add("角色名", 135);

            textBox8.Text = ShowCDKeyInfoLine.ToString();
            textBox10.Text = ShowCDKeyEventLine.ToString();

            label46.Text = "第" + ShowCdkeyInfoPage.ToString() + "页/共" + ShowCdkeyInfoTotalPage.ToString() + "页";
            label42.Text = "第" + ShowCdkeyEventPage.ToString() + "页/共" + ShowCdkeyEventTotalPage.ToString() + "页";
        }


        public void FreshCSServerView(List<CenterServer> serverList)
        {
            CSServerView.Items.Clear();
            CSServerView.BeginUpdate();

            foreach (CenterServer server in serverList)
            {
                ListViewItem item = new ListViewItem(server.Name);
                item.SubItems.Add(server.Ip);
                item.SubItems.Add(server.IfConnect ? "已连接" : "未连接");
                CSServerView.Items.Add(item);
            } 
            CSServerView.EndUpdate();
        }

        public void AddMailServerList(List<CenterServer> serverLis)
        {
            Server_List_Box.CheckOnClick = true;
            Server_List_Box.Items.Add("All");
            foreach (CenterServer cs in serverLis)
            {
                Server_List_Box.Items.Add(cs.Name);
            }

            Server_List_Box.SetItemChecked(0, true);
        }

        public void AddMailTypeList(List<MailTypeCfg> typeList)
        {
            foreach (MailTypeCfg ty in typeList)
            {
                comboBox2.Items.Add(ty.desc);
            }
            comboBox2.BeginUpdate();
            comboBox2.SelectedIndex = 0;
            comboBox2.EndUpdate();
        }

        public void FreshChannelView(List<Channel> channelList)
        {
            checkedListBox1.CheckOnClick = true;
            foreach (Channel sdk in channelList)
            {
                checkedListBox1.Items.Add(sdk.mName);
            }
            checkedListBox1.BeginUpdate();
            checkedListBox1.SetItemChecked(0, true);
            checkedListBox1.EndUpdate();
        }

        private void CSServerView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (CSServerView.SelectedItems.Count == 0) return;
            int i = CSServerView.SelectedItems[0].Index;
            //int i = CSServerView.SelectedIndices.IndexOf(0);
            CenterServer server = Program.serverList[i];
            nowSelectServer = server;
            FreshServerDetails();
            //richTextBox1.Text = server.Name;
            //FreshNoticeBox();
        }

        private void FreshServerDetails()
        {
            lblServerName.Text = nowSelectServer.Name;
            lblOnlineNum.Text = nowSelectServer.UserNum.ToString();
            lblRoomUserNum.Text = nowSelectServer.UserInRoomNum.ToString();
            lblFightUserNum.Text = nowSelectServer.UserInFightNum.ToString();
        }

        public void AddNoticeServerList(List<CenterServer> serverLis)
        {
            server.Items.Clear();
            server.BeginUpdate();
            foreach (CenterServer cs in serverLis)
            {
                server.Items.Add(cs.Name);

            }
            server.SelectedIndex = 0;
            server.EndUpdate();
        }

        public void AddDBQuery(CenterServer DBQuery)
        {
            connectionString += "server=";
            connectionString += DBQuery.Ip;
            connectionString += ";uid=ouyang";
            connectionString += ";pwd=123321";
            connectionString += ";database=fball_accountdb;";
        }

        public void AddNoticePlatformList(List<Channel> channellist)
        {
            channel_notice.Items.Clear();
            channel_notice.BeginUpdate();
            foreach (Channel cs in channellist)
            {
                channel_notice.Items.Add(cs.mName);
                combo_cdkey.Items.Add(cs.mName);
                comboBox1.Items.Add(cs.mName);
            }
            channel_notice.SelectedIndex = 0;
            channel_notice.EndUpdate();

            combo_cdkey.SelectedIndex = 0;
            combo_cdkey.EndUpdate();

            comboBox1.SelectedIndex = 0;
            comboBox1.EndUpdate();
        }

        private void CSServerFreshBtn_Click(object sender, EventArgs e)
        {
            FreshCSServerView(Program.serverList);
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Program.close();
        }

        private void addNoticeBtn_Click(object sender, EventArgs e)
        {
            if (nowSelectServer == null)
                return;

            if (/*GetColorNoticeTitle()*/richTextBox_title.Text.ToString().Length > 8)
            {
                MessageBox.Show("标题太长！最多为八个字！");
                return;
            }

            if (richTextBox_title.Text.ToString().Length < 1)
            {
                MessageBox.Show("标题不能为空！");
                return;
            }

            notice.titel_str = richTextBox_title.Text.ToString();

            GetColorNoticeInfo();

            if (!CheckNoticeIntegrity())
            {
                return;
            }

            GetNoticeDateTime();

            if (notice.end_time <= notice.star_time)
            {
                MessageBox.Show("公告日期有误！");
                return;
            }

            int bFlag = 0;
            if (checkBox2.Checked)
            {
                foreach (CenterServer cs in Program.serverList)
                {
                    bFlag = cs.sendAskAddNotice(notice);
                }
            }
            else
            {
                nowSelectServer.sendAskAddNotice(notice);
            }

            if (bFlag == 0)
            {
                MessageBox.Show("公告发送成功！");
            }
            else
            {
                MessageBox.Show("公告发送失败：没有连接到服务器！");
            }
        }

        private bool CheckNoticeIntegrity()
        {
            if (notice.platform_id == 0)
            {
                MessageBox.Show("请选择一个要发送的平台！");
                return false;
            }
            if (notice.titel_str == null)
            {
                MessageBox.Show("标题不能为空！");
                return false;
            }
            if (notice.flag == 0)
            {
                MessageBox.Show("请选择一个标识！");
                return false;
            }
            if (notice.priority == 0)
            {
                MessageBox.Show("请选择一个排序等级！");
                return false;
            }
            if (notice.notice == null)
            {
                MessageBox.Show("内容不能为空！");
                return false;
            }

            return true;
        }

        private int GetColorNoticeTitle()
        {
            if (richTextBox_title.Text.ToString().Length < 1)
            {
                return 0;
            }

            if (richTextBox_title.Text.ToString().Length > 8)
            {
                return -1;
            }

            Color temp_color = Color.Black;
            Color cur_color = Color.White;
            string s_color = "";
            string send_str = "";

            notice.titel_str = richTextBox_title.Text.ToString();
            for (int i = 0; i < notice.titel_str.Length; i++)
            {
                this.msg_info.Select(i, 1);
                if (i == 0)
                {
                    temp_color = this.richTextBox_title.SelectionColor;
                    s_color = string.Format("{0:X2}{1:X2}{2:X2}", temp_color.R, temp_color.G, temp_color.B);
                    send_str = "[" + s_color + "]" + notice.titel_str[i];
                    continue;
                }
                else
                {
                    cur_color = this.richTextBox_title.SelectionColor;
                }

                if (temp_color == cur_color)
                {
                    send_str += notice.titel_str[i];
                    continue;
                }
                else
                {
                    send_str += "[-]";
                    s_color = string.Format("{0:X2}{1:X2}{2:X2}", cur_color.R, cur_color.G, cur_color.B);
                    send_str += "[" + s_color + "]" + notice.titel_str[i];
                    temp_color = cur_color;
                }
            }

            send_str += "[-]";
            notice.titel_str = send_str;
            return 0;
        }

        private void GetColorNoticeInfo()
        {
            if (msg_info.Text.ToString().Length < 1)
            {
                return;
            }

            //转颜色字格式
            Color temp_color = Color.Black;
            Color cur_color = Color.White;
            string s_color = "";
            string send_str = "";

            int j = 0;
            notice.notice = msg_info.Text.ToString();
            for (int i = 0; i < notice.notice.Length; i++)
            {
                if (j % 25 == 0 && j != 0 && msg_info.Text[i] != '\n')
                {
                    send_str += Environment.NewLine;
                }

                if (msg_info.Text[i] == '\n')
                {
                    j = 0;
                }
                else
                {
                    j++;
                }

                this.msg_info.Select(i, 1);
                if (i == 0)
                {
                    temp_color = this.msg_info.SelectionColor;
                    s_color = string.Format("{0:X2}{1:X2}{2:X2}", temp_color.R, temp_color.G, temp_color.B);
                    send_str = "[" + s_color + "]" + notice.notice[i];
                    continue;
                }
                else
                {
                    cur_color = this.msg_info.SelectionColor;
                }

                if (temp_color == cur_color)
                {
                    send_str += notice.notice[i];
                    continue;
                }
                else
                {
                    send_str += "[-]";
                    s_color = string.Format("{0:X2}{1:X2}{2:X2}", cur_color.R, cur_color.G, cur_color.B);
                    send_str += "[" + s_color + "]" + notice.notice[i];
                    temp_color = cur_color;
                }
            }
            send_str += "[-]";
            notice.notice = send_str;
        }

        private void GetNoticeDateTime()
        {
            DateTime time_star = start_datetime.Value;
            DateTime time_end = end_datetime.Value;

            notice.star_time = DateTimeToTime_t(time_star);
            notice.end_time = DateTimeToTime_t(time_end);
        }

        private void setDefaultSelected(bool b)
        {
            textBox1.Text = "";
            textBox1.Enabled = b;
            button1.Enabled = b;

            checkBox_user_id.Enabled = b;
            checkBox_user_id.Checked = b;

            checkBox_name.Enabled = b;
            checkBox_cdkey.Enabled = b;
        }

        private void setDefaultMailEndTime()
        {
            String mailValidTime = textBox6.Text;
            DateTime dt1 = DateTime.Now;
            DateTime dt2 = dt1.AddDays(Convert.ToInt32(mailValidTime));

            dateTimePicker2.Value = dt2;
            dateTimePicker2.Format = DateTimePickerFormat.Custom;
            dateTimePicker2.CustomFormat = "yyyy-MM-dd HH:mm:ss";
        }

        //点击全选 则用户输入无效
        private void checkBox5_MouseClick(object sender, MouseEventArgs e)
        {
            checkBox6.Checked = false;
            checkBox5.Checked = true;

            setDefaultSelected(false);
        }

        private void checkBox6_MouseClick(object sender, MouseEventArgs e)
        {
            checkBox6.Checked = true;
            checkBox5.Checked = false;

            setDefaultSelected(true);
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (CDKeyMgr.SelectedTab == tabPage7)
            {
                nowSelectServer.AskQueryCDkeyEvents();
            }
            checkBox5.Checked = true;
            checkBox6.Checked = false;
            setDefaultSelected(false);

            button2.Enabled = false;

            DbFunc.CloseConn();
        }

        //获取当前选择的服务器
        private string getCurtChoseServer()
        {
            string server = null;
            for (int i = 0; i < Server_List_Box.Items.Count; i++)
            {
                if (Server_List_Box.GetItemChecked(i))
                {
                    server = (i == 0) ? "All" : checkedListBox1.GetItemText(Server_List_Box.Items[i]);
                    break;
                }
            }
            return server;
        }
        //获取选择的渠道
        private int getCurtChoseChannelId()
        {
            //0表示全选
            int sdcIdx = 0;
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (checkedListBox1.GetItemChecked(i))
                {
                    String choseSdk = checkedListBox1.GetItemText(checkedListBox1.Items[i]);

                    sdcIdx = MailFunc.GetChoseSdkIdx(choseSdk);

                    break;
                }
            }
            return sdcIdx;
        }
        //1,20001,1|2,3005,10
        private string checkHandselStr(String selStr)
        {
            if (selStr.Length < 1)
            {
                return "";
            }
            String resultStr = "";
            String errorStr = "";

            String[] spliteStr = selStr.Split(new char[] { '|' });
            for (int i = 0; i < spliteStr.Length; i++)
            {
                String[] handsel = spliteStr[i].Split(new char[] { ',' });
                if (handsel.Length != 3)
                {
                    errorStr += spliteStr[i] + ";";
                }
                else if (!Program.CheckHandSelType(Convert.ToInt32(handsel[0])))
                {
                    errorStr += spliteStr[i] + ";";
                }
                else
                {
                    resultStr += spliteStr[i] + "|";
                }
            }
            if (errorStr.Length > 0)
            {
                label17.Text = "错误的附件:" + errorStr;
            }

            return resultStr;
        }

        private void ClearUI()
        {
            label17.Text = "";

            textBox1.Text = "";
            textBox2.Text = "";

            if (textBox4.Text.Length < 1)
            {
                textBox3.Text = "";
            }
            textBox5.Text = "";

            //  dateTimePicker1.Text = DateTime.Now.ToString();
            //    dateTimePicker1.Format = DateTimePickerFormat.Custom;
            //   dateTimePicker1.CustomFormat = "yyyy-MM-dd HH:mm:ss";

            //dateTimePicker2.Text = DateTime.Now.ToString();
            //dateTimePicker2.Format = DateTimePickerFormat.Custom;
            //dateTimePicker2.CustomFormat = "yyyy-MM-dd HH:mm:ss";
        }

        private String getMailContent()
        {
            String content = textBox3.Text.Trim();
            if (content.Length < 1)
            {
                return null;
            }
            String contColor = textBox3.ForeColor.R.ToString("x2") + textBox3.ForeColor.G.ToString("x2") + textBox3.ForeColor.B.ToString("x2");
            content = "[" + contColor + "]" + content + "[-]";

            return content;
        }
        private String getMailSender()
        {
            String sender = textBox5.Text.Trim();
            if (sender.Length < 1)
            {
                return null;
            }
            String titleColor = textBox5.ForeColor.R.ToString("x2") + textBox5.ForeColor.G.ToString("x2") + textBox5.ForeColor.B.ToString("x2");
            sender = "[" + titleColor + "]" + sender + "[-]";

            return sender;
        }
        private String getMailTitle()
        {
            String title = textBox2.Text.Trim();
            if (title.Length < 1)
            {
                return null;
            }
            String titleColor = textBox2.ForeColor.R.ToString("x2") + textBox2.ForeColor.G.ToString("x2") + textBox2.ForeColor.B.ToString("x2");
            title = "[" + titleColor + "]" + title + "[-]";

            return title;
        }
        //计算结束时间
        private string GetMailEndTime(string startTime,int day)
        {
            string endtime = null;

            DateTime date = Convert.ToDateTime(startTime);
          date=  date.AddDays(day);

            endtime = date.ToString("yyyy-MM-dd HH:mm:ss"); 

            return endtime;
        }
        private void button3_Click(object sender, EventArgs e)
        {
            String mailTitle = getMailTitle();
            if (mailTitle == null || mailTitle.Length < 1)
            {
                MessageBox.Show("邮件标题不能为空！");
                return;
            }

            String mailContent = getMailContent();
            if (mailContent == null || mailContent.Length < 1)
            {
                MessageBox.Show("邮件内容不能为空！");
                return;
            }

            bool userAll = checkBox5.Checked;
            bool notUserAll = checkBox6.Checked;

            String userList = textBox1.Text;
            if (notUserAll && userList.Length < 1)
            {
                MessageBox.Show("‘部分’用户情况下，用户不能为空!");
                return;
            }

            if (notUserAll && MailFunc.userIdList.Count() < 1)
            {
                MessageBox.Show("‘部分’用户情况下，必须先检查用户是否存在！");
                return;
            }
            bool isAllServer = true;
            string server = getCurtChoseServer();
            if (server == null)
            {
                MessageBox.Show(" 必须勾选服务器列表！");
                return;
            }
            if (server != null && server.CompareTo("All") != 0)
            {
                isAllServer = false;
            }
            int sdkIdx = getCurtChoseChannelId();

            //邮件类型
            int mailType = MailFunc.GetCurtMailTypeIdx(comboBox2.Text);
            //发件人
            String senderText = getMailSender(); //textBox5.Text.Trim();

            String startTime = dateTimePicker1.Value.ToString("yyyy-MM-dd HH:mm:ss");
            int day = Convert.ToInt32(textBox6.Text);

            String endTime = GetMailEndTime(startTime, day); 
            //附件
            String gift = textBox4.Text.Trim();
            gift = checkHandselStr(gift);

            MailInfo mail = new MailInfo();
            mail.init(sdkIdx, mailType, mailTitle, mailContent, gift, startTime, endTime, day, senderText);


            bool bFlag = FormFunc.SendNewMailToCS(nowSelectServer, isAllServer, userAll, MailFunc.userIdList, mail);
            if (!bFlag)
            {
                CS_CONN_INFO.Text = "没有可发送的在线服务器！";
            }
            else
            {
                //  ClearUI();
                MessageBox.Show("发送邮件成功!");
            }

        }

        private void updateUserInput(List<UserInfo> userList, List<String> errList)
        {
            String errorStr = "";
            textBox1.Clear();

            foreach (String str in errList)
            {
                errorStr += str + ",";
            }

            foreach (UserInfo user in userList)
            {
                textBox1.Text += user.obj + ",";
            }

            if (errorStr.Length > 0)
            {
                Tools.CreateLogFile(errorStr);
            }
            else
            {
                MessageBox.Show("检查结束!");
            }

        }
        //检查用户
        private void button1_Click(object sender, EventArgs e)
        {
            string server = getCurtChoseServer();
            bool bAllUser = checkBox6.Checked;
            if (bAllUser && server.CompareTo("All") == 0)
            {
                MessageBox.Show("全部服务器状态下，不能检测'部分用户''!");
                return;
            }

            String user = textBox1.Text.Trim();
            if (user.Length < 1)
            {
                MessageBox.Show("需要输入内容!");
                return;
            }
            int userType = 0;
            if (checkBox_user_id.Checked)
            {
                userType = 1;
            }
            else if (checkBox_cdkey.Checked)
            {
                userType = 3;
            }
            else if (checkBox_name.Checked)
            {
                userType = 2;
            }

            MailInfoCfg dbCfg = MailFunc.GetCurtCSDBInfo(server);

            if (MailFunc.GetUseridAndQuery(userType, user, dbCfg))
            {
                updateUserInput(MailFunc.existUserList, MailFunc.errorStr);

            }
            else if (DbFunc.err.Length > 0)
            {
                MessageBox.Show(DbFunc.err);
            }
        }
        private void button4_Click(object sender, EventArgs e)
        {
            ClearUI();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            ClearUI();
            DbFunc.CloseConn();
            Close();
        }

        private void SetColorDlg(TextBox obj)
        {
            ColorDialog dlg = new ColorDialog();
            dlg.AllowFullOpen = true;
            dlg.FullOpen = true;
            dlg.ShowHelp = true;
            dlg.AnyColor = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                obj.ForeColor = dlg.Color;
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            SetColorDlg(textBox2);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            SetColorDlg(textBox3);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            PrintDocument pdt = new PrintDocument();
            pdt.PrinterSettings = new PrinterSettings();
            pdt.DefaultPageSettings = new PageSettings();
            PageSettings page = pdt.DefaultPageSettings;
            pdt.PrintPage += new PrintPageEventHandler(pdt_PrintPage);

            PrintPreviewDialog dlg = new PrintPreviewDialog();
            dlg.Document = pdt;
            dlg.ShowDialog();
        }
        void pdt_PrintPage(object sender, PrintPageEventArgs e)
        {
            if (textBox2.Text.Length < 1 || textBox3.Text.Length < 1)
                return;
            String handsel = textBox4.Text;
            Color handselColor = textBox4.ForeColor;
            List<String> listHandsel = Tools.SpliteStr(handsel, '|');
            if (listHandsel == null || listHandsel.Count < 1)
            {
                return;
            }

            String strHandset = "";
            for (int i = 0; i < listHandsel.Count; i++)
            {
                strHandset += listHandsel[i] + "\t";
                if ((i + 1) % 2 == 0)
                {
                    strHandset += "\n";
                }
            }
            String mailSender = textBox5.Text;

            String mailSendTime = dateTimePicker1.Value.ToString("yyyy-MM-dd H:m:s");

            Graphics grfx = e.Graphics;
            SolidBrush sbr = new SolidBrush(textBox2.ForeColor);

            int len = textBox2.Text.Length;
            grfx.DrawString(textBox2.Text, new Font("宋体", 12), sbr, 100, 50 + len);

            sbr = new SolidBrush(textBox3.ForeColor);
            len += textBox3.Text.Length;
            grfx.DrawString(textBox3.Text, new Font("宋体", 12), sbr, 100, 100 + len);

            len += mailSendTime.Length;
            grfx.DrawString(mailSendTime, new Font("宋体", 12), Brushes.Black, 100, 150 + len);

            len += mailSender.Length;
            grfx.DrawString(mailSender, new Font("宋体", 12), Brushes.Black, 100, 200 + len);

            len += strHandset.Length;
            grfx.DrawString(strHandset, new Font("宋体", 12), Brushes.Black, 100, 250 + len);
        }


        private void checkBox_user_id_MouseClick(object sender, MouseEventArgs e)
        {
            textBox1.Clear();

            checkBox_name.Checked = false;
            checkBox_cdkey.Checked = false;
            checkBox_user_id.Checked = true;
        }

        private void checkBox_name_MouseClick(object sender, MouseEventArgs e)
        {
            textBox1.Clear();

            checkBox_name.Checked = true;
            checkBox_cdkey.Checked = false;
            checkBox_user_id.Checked = false;
        }

        private void checkBox_cdkey_MouseClick(object sender, MouseEventArgs e)
        {
            textBox1.Clear();

            checkBox_name.Checked = false;
            checkBox_cdkey.Checked = true;
            checkBox_user_id.Checked = false;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            DbFunc.CloseConn();
        }

        private void checkBox9_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_All;

        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_91;
        }

        private void server_SelectedIndexChanged(object sender, EventArgs e)
        {
            DbFunc.CloseConn();
            int i = server.SelectedIndex;
            CenterServer serverid = Program.serverList[i];
            nowSelectServer = serverid;
        }

        private void notice_priority_SelectedIndexChanged(object sender, EventArgs e)
        {
            string t_str = notice_priority.SelectedItem.ToString();
            notice.priority = int.Parse(t_str);
        }

        private void title_colour_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                colorDialog1.AllowFullOpen = true;
                colorDialog1.FullOpen = true;
                colorDialog1.AnyColor = true;
                //richTextBox1_notice_titel.BackColor = Color.Brown;
                richTextBox_title.SelectionColor = colorDialog1.Color;
                //richTextBox_title.ForeColor = colorDialog1.Color;
            }
        }

        private void msg_colour_Click(object sender, EventArgs e)
        {
            //colorDialog1.ShowDialog();
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                //msg_info.BackColor = Color.Brown;
                msg_info.SelectionColor = colorDialog1.Color;
            }
        }

        private void preview_Click(object sender, EventArgs e)
        {
            if (GetColorNoticeTitle() == -1)
            {
                MessageBox.Show("Title too long!");
                return;
            }

            GetColorNoticeInfo();

            if (!CheckNoticeIntegrity())
            {
                return;
            }

            Form1 preview_form = new Form1();
            preview_form.Size = new Size(600, 500);
            preview_form.notice = notice;
            preview_form.Show();
        }


        private void platform_cmge_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_CMGE;
        }

        private void platform_pp_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_PP;
        }

        private void platform_uc_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_UC;
        }

        private void platform_AS_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_As;
        }

        private void platform_iTools_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_iTools;
        }

        private void platform_OG_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_OnlineGame;
        }

        private void platform_tb_CheckedChanged(object sender, EventArgs e)
        {
            notice.platform_id = CenterServer.EPlatform.ePlatform_TB;
        }

        private void flag_sale_CheckedChanged(object sender, EventArgs e)
        {
            if (flag_sale.Checked == true)
            {
                notice.flag = CenterServer.EFlag.eFlag_OnSale;
            }

            if (flag_activity.Checked == true)
            {
                if (flag_sale.Checked == true || flag_notice.Checked == true)
                {
                    flag_activity.Checked = false;
                }
            }

            if (flag_notice.Checked == true)
            {
                if (flag_sale.Checked == true || flag_activity.Checked == true)
                {
                    flag_notice.Checked = false;
                }
            }
        }

        private void flag_activity_CheckedChanged(object sender, EventArgs e)
        {
            if (flag_activity.Checked == true)
            {
                notice.flag = CenterServer.EFlag.eFlag_Services;
            }

            if (flag_notice.Checked == true)
            {
                if (flag_sale.Checked == true || flag_activity.Checked == true)
                {
                    flag_notice.Checked = false;
                }
            }

            if (flag_sale.Checked == true)
            {
                if (flag_activity.Checked == true || flag_notice.Checked == true)
                {
                    flag_sale.Checked = false;
                }
            }
        }

        private void flag_notice_CheckedChanged(object sender, EventArgs e)
        {
            if (flag_notice.Checked == true)
            {
                notice.flag = CenterServer.EFlag.eFlag_Notice;
            }

            if (flag_activity.Checked == true)
            {
                if (flag_sale.Checked == true || flag_notice.Checked == true)
                {
                    flag_activity.Checked = false;
                }
            }

            if (flag_sale.Checked == true)
            {
                if (flag_activity.Checked == true || flag_notice.Checked == true)
                {
                    flag_sale.Checked = false;
                }
            }

        }

        private void state_hot_CheckedChanged(object sender, EventArgs e)
        {
            if (state_hot.Checked == true)
            {
                notice.state = CenterServer.EState.eState_Hot;
            }
            else
            {
                notice.state = CenterServer.EState.eState_None;
            }

            if (state_hot.Checked == true && state_new.Checked == true)
            {
                state_new.Checked = false;
            }
        }

        private void state_new_CheckedChanged(object sender, EventArgs e)
        {
            if (state_new.Checked == true)
            {
                notice.state = CenterServer.EState.eState_New;
            }
            else
            {
                notice.state = CenterServer.EState.eState_None;
            }

            if (state_hot.Checked == true && state_new.Checked == true)
            {
                state_hot.Checked = false;
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            msg_info.SelectionAlignment = HorizontalAlignment.Left;
        }

        private void button9_Click(object sender, EventArgs e)
        {
            msg_info.SelectionAlignment = HorizontalAlignment.Center;
        }

        private void button10_Click(object sender, EventArgs e)
        {
            msg_info.SelectionAlignment = HorizontalAlignment.Right;
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            //server.Enabled = false;
            DbFunc.CloseConn();
            int i = channel_notice.SelectedIndex;
            Channel channel = Program.channelList[i];
            notice.platform_id = (CenterServer.EPlatform)channel.mIndex;
        }

        private void button11_Click(object sender, EventArgs e)
        {
            if (CDKeyItem > 1)
            {
                CDKeyItemText[CDKeyItem - 1].Visible = false;
                CDKeyItemNumText[CDKeyItem - 1].Visible = false;
                CDKeyItem--;
                button12.Location = new Point(265, 564 + CDKeyItem * 30 - 30);
                button14.Location = new Point(451, 564 + CDKeyItem * 30 - 30);
            }
        }

        private void button13_Click(object sender, EventArgs e)
        {
            if (CDKeyItem < 5)
            {
                CDKeyItemText[CDKeyItem].Visible = true;
                CDKeyItemNumText[CDKeyItem].Visible = true;
                CDKeyItem++;
                button12.Location = new Point(252, 565 + CDKeyItem * 30 - 30);
                button14.Location = new Point(442, 565 + CDKeyItem * 30 - 30);
            }
        }

        private void checkBox2_CheckedChanged_1(object sender, EventArgs e)
        {
            if (checkBox2.Checked == true)
            {
                server.Enabled = false;
            }
            else
            {
                server.Enabled = true;
            }
        }

        private void textBox8_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void textBox12_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void textBox13_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void textBox9_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsLetter(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void textBox10_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void button12_Click(object sender, EventArgs e)
        {
            //CDKey标题
            CDKeyInfo.title = CDKey_title.Text.ToString();
            if (CDKeyInfo.title.Length < 1)
            {
                MessageBox.Show("标题不能为空！");
                return;
            }
            //CDKey对应平台
            Channel channel = Program.channelList[combo_cdkey.SelectedIndex];
            CDKeyInfo.platform = (CenterServer.EPlatform)channel.mIndex;
            if (CDKeyInfo.platform == CenterServer.EPlatform.ePlatform_None)
            {
                MessageBox.Show("请选择一个有效平台！");
                return;
            }
            //产生CDKey数量
            if (ProduceCDKeyNum.Text.ToString().Length != 0)
            {
                CDKeyInfo.CDKeyNum = Convert.ToInt32(ProduceCDKeyNum.Text.ToString());
            }
            else
            {
                MessageBox.Show("请输入生成数量！");
                return;
            }
            //以某字母开头
            CDKeyInfo.letter = CDKeyFlag.Text.ToString();
            if (CDKeyInfo.letter.Length > 1)
            {
                MessageBox.Show("兑换码过长！");
                return;
            }
            //CDKey长度
            int length = 0;
            if (CDKeyLen.Text.ToString().Length != 0)
            {
                CDKeyInfo.CDKeyLen = Convert.ToInt32(CDKeyLen.Text.ToString());
                if (CDKeyInfo.CDKeyLen < 12)
                {
                    MessageBox.Show("CDKey长度过短!");
                    return;
                }
            }
            else
            {
                MessageBox.Show("请输入要生成长度!");
                return;
            }
            //获取CDK过期时间
            getCDKeyEndTime();
            //CDK用处说明
            CDKeyInfo.gift_msg = CDKey_MSG.Text.ToString();
            //礼包类型
            //CDKeyInfo.gift_type = comboBox4.SelectedIndex;
            CDKeyInfo.gift_type = 0;
            //礼包包含物品
            CDKeyInfo.item_id = new int[6];
            CDKeyInfo.item_num = new int[6];
            CDKeyInfo.gift_num = 0;
            for (int i = 0; i < CDKeyItem; i++)
            {
                if (CDKeyItemText[i].Visible == false)
                {
                    continue;
                }
                if (CDKeyItemText[i].Text.ToString().Length == 0)
                {
                    MessageBox.Show("物品Id不能为空！");
                    return;
                }
                CDKeyInfo.gift_num++;
                CDKeyInfo.item_id[i] = Convert.ToInt32(CDKeyItemText[i].Text.ToString());

                if (CDKeyItemNumText[i].Text.ToString().Length == 0)
                {
                    MessageBox.Show("物品数量不能为空！");
                    return;
                }
                CDKeyInfo.item_num[i] = Convert.ToInt32(CDKeyItemNumText[i].Text.ToString());
            }

            int ret = nowSelectServer.sendCreateCDKeyEventPara(CDKeyInfo);
            if (ret == 0)
            {
                MessageBox.Show("success!");
            }
        }


        private void getCDKeyEndTime()
        {
            //end_time
            DateTime cdkey_end_time = cdkey_date.Value;
            CDKeyInfo.end_time = DateTimeToTime_t(cdkey_end_time);
        }

        private void button15_Click(object sender, EventArgs e)
        {
            SetColorDlg(textBox5);
        }

        private void checkedListBox1_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (i != e.Index)
                {
                    checkedListBox1.SetItemChecked(i, false);
                }
            }
        }

        private void button16_Click(object sender, EventArgs e)
        {
            string str = "";
            if (textBox7.Text.ToString().Length != 0)
            {
                str = textBox7.Text.ToString();
            }

            Channel channel = Program.channelList[comboBox1.SelectedIndex];
            CenterServer.EPlatform platform = (CenterServer.EPlatform)channel.mIndex;

            int count = 0;
            string plat_str = "";
            foreach (CenterServer.SCDKey_Event c in CDkey_Events)
            {
                if (c.plat == (int)platform && -1 != c.title.IndexOf(str))
                {
                    if (count == 0)
                    {
                        listView_CDkey.Items.Clear();
                    }

                    ListViewItem item = new ListViewItem(c.title);
                    plat_str = GetPlatformName(c.plat);
                    item.SubItems.Add(plat_str);           
                    DateTime t = Time_tToDateTime(c.end_time);
                    item.SubItems.Add(t.ToString());
                    item.SubItems.Add(c.code_num.ToString());
                    item.SubItems.Add(c.code_len.ToString());
                    item.SubItems.Add(c.type.ToString());
                    listView_CDkey.Items.Add(item);
                    ++count;
                }
            }

            if (count == 0)
            {
                MessageBox.Show("No match events!");
            }

        }

        private void cdkey_add_Click(object sender, EventArgs e)
        {
            CDKeyMgr.SelectedIndex = 5;
            //tabControl1_SelectedIndexChanged
        }


        private void listView_CDkey_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            e.Cancel = true;
            e.NewWidth = listView_CDkey.Columns[e.ColumnIndex].Width;
        }

        private void button25_Click(object sender, EventArgs e)
        {
            if (search_event_id == 0)
            {
                MessageBox.Show("请选择一个目标！");
                return;
            }

            if (textBox8.Text.ToString().Length > 0)
            {
                ShowCDKeyInfoLine = Convert.ToInt32(textBox8.Text.ToString());
            }

            QueryCDKeyInfo();

            GetTotalCDKeyInfoPages();
            
            ShowCdkeyInfoPage = 1;
            ShowCDKeyInfos();
            CDKeyMgr.SelectedIndex = 7;
        }

        private void GetTotalCDKeyInfoPages()
        {
            double temp = (double)CDkey_Infos.Count / (double)ShowCDKeyInfoLine;
            ShowCdkeyInfoTotalPage = (int)Math.Ceiling(temp);
        }

        private void GetTotalCDKeyEventPages()
        {
            double temp = (double)CDkey_Events.Count / (double)ShowCDKeyEventLine;
            ShowCdkeyEventTotalPage = (int)Math.Ceiling(temp);
        }

        public void FreshCDKeyEvents()
        {
            listView_CDkey.Items.Clear();
            int counter = 0;
            foreach (CenterServer.SCDKey_Event events_info in CDkey_Events)
            {
                ++counter;
                if (counter <= ShowCDKeyEventLine * (ShowCdkeyEventPage - 1))
                {
                    continue;
                }

                ListViewItem item = new ListViewItem(events_info.title);

                item.SubItems.Add(GetPlatformName(events_info.plat));
                DateTime temp = Time_tToDateTime(events_info.end_time);
                item.SubItems.Add(temp.ToString());
                item.SubItems.Add(events_info.code_num.ToString());
                item.SubItems.Add(events_info.code_len.ToString());
                item.SubItems.Add(events_info.type.ToString());
                listView_CDkey.Items.Add(item);

                if (counter >= ShowCdkeyEventPage * ShowCDKeyEventLine)
                {
                    break;
                }
            }
            if (ShowCdkeyEventPage > ShowCdkeyEventTotalPage)
            {
                ShowCdkeyEventPage = 1;
            }

            label42.Text = "第" + ShowCdkeyEventPage.ToString() + "页/共" + ShowCdkeyEventTotalPage.ToString() + "页";
        }

        private void Server_List_Box_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            for (int i = 0; i < Server_List_Box.Items.Count; i++)
            { 
                if (i != e.Index)
                {
                    Server_List_Box.SetItemChecked(i, false); 
                }
                
            } 
        }

        public void ShowCDKeyInfos()
        {
            int counter = 0;
            CDKeyView.Items.Clear();
            foreach (CenterServer.SCDKey_Info cdkey_info in CDkey_Infos)
            {
                ++ counter;
                if (counter <= ShowCDKeyInfoLine * (ShowCdkeyInfoPage - 1))
                {
                    continue;
                }

                ListViewItem item = new ListViewItem(cdkey_info.key);
                item.SubItems.Add(cdkey_info.use_state.ToString());
                if (cdkey_info.use_time == 0)
                {
                    item.SubItems.Add("未使用");
                } 
                else
                {
                    DateTime temp = Time_tToDateTime(cdkey_info.use_time);
                    item.SubItems.Add(temp.ToString());
                }
                item.SubItems.Add(cdkey_info.server_id.ToString());
                item.SubItems.Add(cdkey_info.user_name);
                CDKeyView.Items.Add(item);

                if (counter >= ShowCDKeyInfoLine * ShowCdkeyInfoPage)
                {
                    break;
                }
            }

            if (ShowCdkeyInfoPage > ShowCdkeyInfoTotalPage)
            {
                ShowCdkeyInfoPage = 1;
            }

            label46.Text = "第" + ShowCdkeyInfoPage.ToString() + "页/共" + ShowCdkeyInfoTotalPage.ToString() + "页";
        }

        public static long DateTimeToTime_t(DateTime dateTime)
        {
            long time_t;
            DateTime dt1 = new DateTime(1970, 1, 1, 0, 0, 0);
            TimeSpan ts = dateTime - dt1;
            time_t = ts.Ticks / 10000000 - 28800;
            return time_t;
        }

        public void QueryCDKeyEvents()
        {
            CDkey_Events.Clear();
            /*string connectionString = "server=localhost;uid=root;pwd=123321;database=fball_accountdb;";*/
            string queryString = "select * from cdkey_events;";
            using (MySqlConnection connection = new MySqlConnection(connectionString))
            {
                MySqlCommand command = connection.CreateCommand();
                command.CommandText = queryString;
                try
                {
                    connection.Open();
                    CenterServer.SCDKey_Event event_info;
                    MySqlDataReader reader = command.ExecuteReader();
                    while (reader.Read())
                    {
                        event_info.id = Convert.ToInt32(reader.GetValue(0).ToString());
                        event_info.title = reader.GetValue(1).ToString();
                        event_info.plat = Convert.ToInt32(reader.GetValue(2).ToString());
                        event_info.end_time = Convert.ToInt64(reader.GetValue(3).ToString());
                        event_info.content = reader.GetValue(4).ToString();
                        event_info.type = Convert.ToInt32(reader.GetValue(5).ToString());
                        event_info.item = reader.GetValue(6).ToString();
                        event_info.code_num = Convert.ToInt32(reader.GetValue(7).ToString());
                        event_info.code_len = Convert.ToInt32(reader.GetValue(8).ToString());
                        CDkey_Events.Add(event_info);
                    }
                    reader.Close();
                    connection.Close();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
            FreshCDKeyEvents();
        }

        public void QueryCDKeyInfo()
        {
            CDkey_Infos.Clear();
            string queryString = "select * from cdkey_info where event_id=" + search_event_id.ToString() + ";";
            using (MySqlConnection connection = new MySqlConnection(connectionString))
            {
                MySqlCommand command = connection.CreateCommand();
                command.CommandText = queryString;
                try
                {
                    connection.Open();
                    CenterServer.SCDKey_Info cdkey_info;
                    MySqlDataReader reader = command.ExecuteReader();
                    while (reader.Read())
                    {
                        cdkey_info.key = reader.GetValue(0).ToString();
                        cdkey_info.event_id = Convert.ToInt32(reader.GetValue(1).ToString());
                        cdkey_info.use_state = Convert.ToInt32(reader.GetValue(2).ToString());
                        cdkey_info.use_time = Convert.ToInt64(reader.GetValue(3).ToString());
                        cdkey_info.server_id = Convert.ToInt32(reader.GetValue(4).ToString());
                        cdkey_info.user_name = reader.GetValue(5).ToString();
                        CDkey_Infos.Add(cdkey_info);
                    }
                    reader.Close();
                    connection.Close();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }

        public string GetPlatformName(int plat)
        {
            int i = comboBox1.SelectedIndex;
            Channel channel = Program.channelList[i];
            return channel.mName;
        }

        public int GetPlatformID(string plat_name)
        {
            foreach (Channel channel in Program.channelList)
            {
                if (channel.mName.Equals(plat_name))
                {
                    return channel.mIndex;
                }
            }
            return -1;
        }

        static DateTime Time_tToDateTime(long time_t)
        {
            //time_t是世界时间， 比 本地时间 少8小时(即28800秒)
            double seconds = time_t + 28800;
            double secs = Convert.ToDouble(seconds);
            DateTime dt = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(secs);
            //TimeSpan span = 
            //        TimeSpan.FromTicks(seconds*TimeSpan.TicksPerSecond); 
            return dt;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            dateTimePicker2.Text = DateTime.Now.ToString();
            dateTimePicker2.Format = DateTimePickerFormat.Custom;
            dateTimePicker2.CustomFormat = "yyyy-MM-dd HH:mm:ss"; 

            //左边服务器连接框
            FreshCSServerView(Program.serverList);
        } 
        private void Server_List_Box_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selectIp = getCurtChoseServer();
            if (selectIp != null && selectIp.CompareTo("All") == 0)
            { 
                Chose_server_State.Text = "";

                dateTimePicker1.Text = DateTime.Now.ToString();
                dateTimePicker1.Format = DateTimePickerFormat.Custom;
                dateTimePicker1.CustomFormat = "yyyy-MM-dd HH:mm:ss";  
                return;
            }
            Chose_server_State.Text = "";
            foreach (CenterServer cs in Program.serverList)
            {
                if (cs.Name.CompareTo(selectIp) == 0)
                {
                    if (cs.IfConnect == false)
                    {
                        Chose_server_State.Text = "与该服务器断开!";
                    }
                    dateTimePicker1.Text = cs.ServerCurtTime;
                    break;
                }
            }

           nowSelectServer = Program.GetSelectServer(selectIp);  
        }

        private void button22_Click(object sender, EventArgs e)
        {
            if (CDkey_Infos.Count > 0)
            {
                int count = 0;
                StreamWriter text;
                text = File.CreateText(CreateFileName(""));
                foreach (CenterServer.SCDKey_Info info in CDkey_Infos)
                {
                    try
                    {
                        ++count;
                        text.Write(info.key);

                        if (count % 50 == 0)
                        {
                            text.Write('\r');
                            text.Write('\n');
                        }
                        else
                        {
                            text.Write('\t');
                        }

                        if (count == CDkey_Infos.Count)
                        {
                            text.Write(count.ToString());
                        }
                        //++count;
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        return;
                    }
                }
                text.Close();
            }
            else
            {
                MessageBox.Show("当前无可导出兑换码！");
                return;
            }

            MessageBox.Show("兑换码导出完成！");
        }

        public int GetEventID(string title, string plat)
        {
            int platform = 0;
            foreach (Channel channel in Program.channelList)
            {
                if (channel.mName.Equals(plat))
                {
                    platform = channel.mIndex;
                    break;
                }
            }

            foreach (CenterServer.SCDKey_Event info in CDkey_Events)
            {
                if (info.title.Equals(title) && info.plat == platform)
                {
                    return info.id;
                }
            }

            return 0;
        }

        private void listView_CDkey_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = 0;
            foreach (ListViewItem item in listView_CDkey.Items)
            {
                if (item.Selected)
                {
                    search_event_id = GetEventID(listView_CDkey.Items[index].SubItems[0].Text, listView_CDkey.Items[index].SubItems[1].Text);
                    return;
                }
                ++index;
            }
        }

        private void button26_Click(object sender, EventArgs e)
        {
            listView_CDkey.Items.Clear();
            //CDKeyView.Items.Clear();
            QueryCDKeyEvents();
        }

        private void listView_CDkey_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            foreach (ListViewItem item in listView_CDkey.Items)
            {
                if (item.Selected)
                {
                    search_event_id = GetCDKEventID(item.SubItems[0].Text, item.SubItems[1].Text);
                    break;
                }
            }
        }

        private int GetCDKEventID(string title, string platform)
        {
            foreach (CenterServer.SCDKey_Event event_info in CDkey_Events)
            {
                if (event_info.title.Equals(title) && event_info.plat == GetPlatformID(platform))
                {
                    return event_info.id;
                }
            }
            return 0;
        }

        private void button28_Click(object sender, EventArgs e)
        {
            if (CDkey_Infos.Count > 0)
            {
                int count = 0;
                StreamWriter text;
                text = File.CreateText(CreateFileName("未使用"));
                foreach (CenterServer.SCDKey_Info info in CDkey_Infos)
                {
                    try
                    {
                        if (info.use_state != 0)
                        {
                            continue;
                        }

                        ++count;
                        text.Write(info.key);

                        if (count % 50 == 0)
                        {
                            text.Write('\r');
                            text.Write('\n');
                        }
                        else
                        {
                            text.Write('\t');
                        }

                        if (count == CDkey_Infos.Count)
                        {
                            text.Write(count.ToString());
                        }
                        //++count;
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        return;
                    }
                }
                text.Close();
            }
            else
            {
                MessageBox.Show("当前无可导出兑换码！");
                return;
            }

            MessageBox.Show("兑换码导出完成！");
        }

        private string CreateFileName(string header)
        {
            CenterServer.SCDKey_Info temp = CDkey_Infos[0];
            string title = "../";

            if (header.Length > 0)
            {
                title += header;
            }

            foreach (CenterServer.SCDKey_Event event_info in CDkey_Events)
            {
                if (temp.event_id == event_info.id)
                {
                    title += event_info.title + ".txt";
                    break;
                }
            }
            return title;
        }

        private void textBox8_KeyPress_1(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                if (e.KeyChar == '\r')
                {
                    if (textBox8.Text.ToString().Length > 0 && Convert.ToInt32(textBox8.Text.ToString()) > 0)
                    {
                        ShowCDKeyInfoLine = Convert.ToInt32(textBox8.Text.ToString());
                        GetTotalCDKeyInfoPages();
                        ShowCDKeyInfos();
                    }
                }
                e.Handled = true;
            }
        }

        private void CDKeyView_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            e.Cancel = true;
            e.NewWidth = CDKeyView.Columns[e.ColumnIndex].Width;
        }

        private void button29_Click(object sender, EventArgs e)
        {
            if (ShowCdkeyInfoPage < ShowCdkeyInfoTotalPage)
            {
                ++ShowCdkeyInfoPage;
                ShowCDKeyInfos();
            }
        }

        private void button27_Click(object sender, EventArgs e)
        {
            if (ShowCdkeyInfoPage > 1)
            {
                --ShowCdkeyInfoPage;
                ShowCDKeyInfos();
            }
        }

        private void textBox9_KeyPress_1(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void button30_Click(object sender, EventArgs e)
        {
            if (textBox9.Text.ToString().Length > 0 && Convert.ToInt32(textBox9.Text.ToString()) > 0
                && Convert.ToInt32(textBox9.Text.ToString()) <= ShowCdkeyInfoTotalPage)
            {
                ShowCdkeyInfoPage = Convert.ToInt32(textBox9.Text.ToString());
                ShowCDKeyInfos();
            }
        }

        private void textBox10_KeyPress_1(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                if (e.KeyChar == '\r')
                {
                    if (textBox10.Text.ToString().Length > 0 && Convert.ToInt32(textBox10.Text.ToString()) > 0)
                    {
                        ShowCDKeyEventLine = Convert.ToInt32(textBox10.Text.ToString());
                        GetTotalCDKeyEventPages();
                        FreshCDKeyEvents();
                    }
                }
                e.Handled = true;
            }
        }

        private void button18_Click(object sender, EventArgs e)
        {
            if (ShowCdkeyEventPage > 1)
            {
                --ShowCdkeyEventPage;
                FreshCDKeyEvents();
            }
        }

        private void button19_Click(object sender, EventArgs e)
        {
            if (ShowCdkeyEventPage < ShowCdkeyEventTotalPage)
            {
                ++ShowCdkeyEventPage;
                FreshCDKeyEvents();
            }
        }

        private void button21_Click(object sender, EventArgs e)
        {
            if (textBox11.Text.ToString().Length > 0 && Convert.ToInt32(textBox11.Text.ToString()) > 0
                && Convert.ToInt32(textBox11.Text.ToString()) <= ShowCdkeyEventTotalPage)
            {
                ShowCdkeyEventPage = Convert.ToInt32(textBox11.Text.ToString());
                FreshCDKeyEvents();
            }
        }

        private void textBox11_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

    }
}
