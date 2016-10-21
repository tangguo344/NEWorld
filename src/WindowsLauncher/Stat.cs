using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Management;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Net;

namespace NEWorldLauncher
{
    public static class Stat
    {
        static bool allow = false;
        public static void ask()
        {
            string path = Path.GetFullPath(Def.statSettings);
            if (File.Exists(path))
            {
                allow = File.ReadAllText(path) == "True";
                return;
            }
            allow = MessageBox.Show("您是否允许我们收集一些匿名数据？(安全、无关隐私)", "NEWorldLauncher", MessageBoxButtons.YesNo) == DialogResult.Yes;
            File.WriteAllText(path, allow.ToString());
        }
        public static void collectOpenGL()
        {
            if (!allow) return;
            if (File.Exists(Path.GetFullPath(Def.glewinfoOutput))) ;
            var glewinfoProcess = new Process();
            glewinfoProcess.StartInfo.FileName = Path.GetFullPath(Def.glewinfo);
            glewinfoProcess.EnableRaisingEvents = true;
            glewinfoProcess.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            glewinfoProcess.Exited += delegate (object sender, EventArgs e)
            {
                string openglInfo = File.ReadAllText(Path.GetFullPath(Def.glewinfoOutput));
                string macAddress = getMacAddress();

                var request = (HttpWebRequest)WebRequest.Create(Def.reportAddress);

                var postData = "mac=" + macAddress + "&info=" + openglInfo;
                var data = Encoding.ASCII.GetBytes(postData);

                request.Method = "POST";
                request.ContentType = "application/x-www-form-urlencoded";
                request.ContentLength = data.Length;

                using (var stream = request.GetRequestStream())
                {
                    stream.Write(data, 0, data.Length);
                }

                glewinfoProcess.Dispose();
            };
            glewinfoProcess.Start();
        }
        public static string getMacAddress()
        {
            ManagementObjectCollection moc = new ManagementClass("Win32_NetworkAdapterConfiguration").GetInstances();
            foreach (ManagementObject mo in moc)
                if ((bool)mo["IPEnabled"]) return mo["MacAddress"].ToString();
            return "";
        }
    }
}
