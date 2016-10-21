using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace NEWorldLauncher
{
    public partial class DebugForm : Form
    {
        Process process;

        public DebugForm(Game game)
        {
            process = game.process;
            process.Exited += Process_Exited;
            process.OutputDataReceived += Process_OutputDataReceived;
            process.BeginOutputReadLine();
            InitializeComponent();
        }

        private void Process_Exited(object sender, EventArgs e)
        {
            Invoke((Action)delegate
            {
                outTextBox.AppendText(string.Format("游戏退出，返回值是[{0}]\n", process.ExitCode));
                killButton.Text = "已退出";
                killButton.Enabled = false;
            });
        }

        private void Process_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != null)
            {
                Invoke((Action)delegate
                {
                    outTextBox.AppendText(e.Data + '\n');
                });
            }
        }

        private void DebugForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!process.HasExited)
            {
                if (MessageBox.Show("确定要停止调试？", "调试面板", MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    process.Kill();
                    process.Dispose();
                }
                else
                    e.Cancel = true;
            }
            else
                process.Dispose();
        }

        private void killButton_Click(object sender, EventArgs e)
        {
            if (!process.HasExited)
                process.Kill();
        }
    }
}
