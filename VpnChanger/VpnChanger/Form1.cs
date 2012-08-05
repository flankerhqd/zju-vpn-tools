/*
 *
 * http://myvpn.zju.edu.cn:9080/leo/tools/geteducation.action?identityId=54
 * 1 1:本科生
2 2:硕士研究生 3:博士研究生
3 4:International student 5:成人教育学生 6:远程教育学生 7:进修生 8:实习生 35:访问学者 36:其他学生
4 9:教授 10:副教授 11:讲师 12:助教 13:高级实验师 14:实验师 15:助理实验师 16:实验员 17:高高工 18:高级工程师 19:工程师 20:助理工程师 21:技术员 22:主任医师 23:副主任医师 24:主治医师 25:医师 26:医士 27:研究员 28:副研究员 29:助理研究员 30:研究实习员 31:其他高级职务 32:其他中级职务 33:其他初级职务 34:其他一般人员
5 40:教授 41:副教授 42:讲师 43:助教 44:高级实验师 45:实验师 46:助理实验师 47:实验员 48:高高工 49:高级工程师 50:工程师 51:助理工程师 52:技术员 53:主任医师 54:副主任医师 55:主治医师 56:医师 57:医士 58:研究员 59:副研究员 60:助理研究员 61:研究实习员 62:其他高级职务 63:其他中级职务 64:其他初级职务 65:其他一般人员
6 37:留学生 38:交换生 39:其他
 * 
 * http://myvpn.zju.edu.cn:9080/leo/tools/getplan.action?planId=53
 * 
 * Those thanks to @libmaru
 * 52 国外代理 10.0 元/月
53 10元包月 10.0 元/月
54 学生50元包月 50.0 元/月
55 网通反向 0.0 元/月
56 电信反向 0.0 元/月
57
58 教工50元包月 50.0 元/月
59
60
61 教育50元 50.0 元/月
62 数学用户 0.0 元/月
63 邮箱用户 0.0 元/月
64 校园通道 0.0 元/月
65 免费试用 10.0 元/月
66 集团用户(国内代理) 0.0 元/月
67 10元包月(国内代理+网通反向) 10.0 元/月
68 10元包月(国内代理+电信反向) 10.0 元/月
69 10元包月(策略全选) 10.0 元/月
70 教工50元包月(电信代理+电信反向) 50.0 元/月
71 教工50元包月(电信代理+网通反向) 50.0 元/月
72 教工50元包月(策略全选) 50.0 元/月
73 学生50元包月(网通代理+电信反向) 50.0 元/月
74 学生50元包月(网通代理+网通反向) 50.0 元/月
75 内部套餐(免费) 0.0 元/月
76 国外代理(+电信反向) 10.0 元/月
77 国外代理(+网通反向) 10.0 元/月
78 国外代理(策略全选) 10.0 元/月
79 电信反向(策略全选) 0.0 元/月
80 网通反向(策略全选) 0.0 元/月
81 集团用户(策略全选) 0.0 元/月
82 其他类型 0.0 元/月
83 10元包月(内部套餐) 0.0 元/月
84 测试套餐(20100131) 0.0 元/月
85
86 rvpn 0.0 元/月
87 2M独享线路 200.0 元/月
88 1M独享线路 100.0 元/月
89 30元包月 30.0 元/月
90 80元包月 80.0 元/月
91 120元包月 120.0 元/月
92 校园通道 0.0 元/月
93 VPN通道 50.0 元/月
*/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Web;
using System.IO;
using System.Text.RegularExpressions;

namespace VpnChanger
{
    public partial class Form1 : Form
    {
        String id, password, checkCode;
        const String URL_HEAD = "http://myvpn.zju.edu.cn:9080/leo/";
        public Form1()
        {
            InitializeComponent();
            this.button2.Enabled = false;
        }
        HttpWebRequest req;
        private void button1_Click(object sender, EventArgs e)
        {
            id = this.idTextBox.Text;
            password = this.passwordTextBox.Text;
            req = (HttpWebRequest)HttpWebRequest.Create("http://myvpn.zju.edu.cn:9080/leo/j_security_check");
            req.Method = "POST";
            req.Referer = "http://myvpn.zju.edu.cn:9080/leo/login.action";
            req.CookieContainer = cookie;
            Encoding utf = Encoding.GetEncoding("GB2312");
            String postCnt = HttpUtility.UrlEncode("j_username", utf) + "="
                + HttpUtility.UrlEncode(id, utf) + "&" + HttpUtility.UrlEncode("j_password", utf) + "="
                + HttpUtility.UrlEncode(password, utf) + "&" + HttpUtility.UrlEncode("submit", utf) + "="
                + HttpUtility.UrlEncode("登陆", utf);
            byte[] post = Encoding.ASCII.GetBytes(postCnt);
            req.ContentType =
"application/x-www-form-urlencoded;charset=gb2312";
            req.ContentLength = post.Length;  
            using (Stream reqStream = req.GetRequestStream())
            {
                
                reqStream.Write(post, 0, post.Length);
            }

            WebResponse wr = req.GetResponse();
            {
                Stream stream = wr.GetResponseStream();
                StreamReader reader = new StreamReader(stream);
                String res = reader.ReadToEnd();
                if (res.Contains(id))
                {
                    MessageBox.Show("登陆成功!");
                    Regex r = new Regex("<input type=\"hidden\" name=\"account.id\" value=\"(\\d+)\"/>");
                    String changePage = getUrlContent("http://myvpn.zju.edu.cn:9080/leo/user/change.action");
                    Match m = r.Match(changePage);
                    if (m.Success)
                        accountId = (r.Match(changePage).Groups[1].ToString());
                    HttpWebRequest imgReq = (HttpWebRequest)HttpWebRequest.Create("http://myvpn.zju.edu.cn:9080/leo/kaptcha.jpg");
                    imgReq.Method = "GET";
                    imgReq.CookieContainer = cookie;
                    cookie = imgReq.CookieContainer;
                    using (WebResponse img = imgReq.GetResponse())
                    {
                        using (Stream receive = img.GetResponseStream())
                        {
                                this.imageLabel.Image = Image.FromStream(receive);
                        }
                    }
                    
                    this.button2.Enabled = true;
                }
                else
                    MessageBox.Show("登陆失败");
            }
        }
        CookieContainer cookie = new CookieContainer();
        private String getUrlContent(String s)
        {
            HttpWebRequest tmp = (HttpWebRequest)HttpWebRequest.Create(s);
            tmp.Method = "GET";
            tmp.Referer = "http://myvpn.zju.edu.cn:9080/leo/user/userinfo.action";
            tmp.CookieContainer = cookie;
            StreamReader reader = new StreamReader(tmp.GetResponse().GetResponseStream());
            return reader.ReadToEnd();
        }
        string[] vpnType = { "75","89","54"};
        string[] vpnStr = { "10.0", "30.0", "50.0" };
        String accountId = "";
        private void button2_Click(object sender, EventArgs e)
        {
           HttpWebRequest tmp = (HttpWebRequest)HttpWebRequest.Create("http://myvpn.zju.edu.cn:9080/leo/user/changeService.action");
           tmp.Method = "POST";
           tmp.ContentType = "application/x-www-form-urlencoded; charset=UTF-8";
           tmp.CookieContainer = cookie;
           String param = "account.id=" + accountId + "&account.plan.planId=" + vpnType[this.typeComboBox.SelectedIndex] + "&sc=" + this.checkCodeTextBox.Text;
           byte[] bs = Encoding.ASCII.GetBytes(param);
            using (Stream reqStream = tmp.GetRequestStream())
           {
               reqStream.Write(bs, 0, bs.Length);
           }
            String response = new StreamReader(tmp.GetResponse().GetResponseStream()).ReadToEnd();
            MessageBox.Show("请查看修改结果");
            //if (response.Contains(vpnStr[this.typeComboBox.SelectedIndex]))
            //    MessageBox.Show("修改成功！");
            //else
            //    MessageBox.Show("修改失败..." + response);
        }

    }
}
