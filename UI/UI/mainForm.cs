using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using Newtonsoft.Json.Linq;
using System.Net;
using System.Net.Sockets;
using Newtonsoft.Json;


namespace UI
{
    public partial class mainForm : Form
    {
        static Process server;
        public mainForm()
        {
            StartServer();
            AppDomain.CurrentDomain.ProcessExit += (s, e) => StopServer();
            Console.WriteLine("Server is starting...");
            // Delay 2 seconds to allow server to start
            System.Threading.Thread.Sleep(2000);
            InitializeComponent();
            
        }
        static void StartServer()
        {
            server = new Process();
            server.StartInfo.FileName = "python"; // or full path like "C:\\Python311\\python.exe"
            server.StartInfo.Arguments = "D:\\university\\ky8_last\\DATN\\prjCsharp\\FR_Server\\fr_server.py"; // replace with your script
            server.StartInfo.CreateNoWindow = true;
            server.StartInfo.UseShellExecute = false;
            server.StartInfo.RedirectStandardOutput = true;
            server.StartInfo.RedirectStandardError = true;

            server.OutputDataReceived += (s, e) => { if (e.Data != null) Console.WriteLine("[SERVER] " + e.Data); };
            server.ErrorDataReceived += (s, e) => { if (e.Data != null) Console.Error.WriteLine("[SERVER ERROR] " + e.Data); };

            server.Start();
            server.BeginOutputReadLine();
            server.BeginErrorReadLine();

            Console.WriteLine("Server started.");
        }

        static void StopServer()
        {
            if (server != null && !server.HasExited)
            {
                Console.WriteLine("Stopping server...");
                server.Kill(); // Kill the server process
                server.Dispose();
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
        }
        string hostIP = Host.IP;
        private void Form1_Load(object sender, EventArgs e)
        {
            pictureBox1.Image = Image.FromFile("D:\\university\\ky8_last\\DATN\\papers\\csharpInProgess\\hardware.jpg");
            warningLabel.Hide();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {
            verifyFeatureForm verifyForm = new verifyFeatureForm();
            verifyForm.ShowDialog();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            addFaceForm passForm = new addFaceForm();

            passForm.ShowDialog();
        }
        private void Form1_TextChanged(object sender, EventArgs e)
        {
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string url = "https://drive.google.com/drive/folders/1N3mfuod80M8ONk3hJMpkmLY8MYu-y8Qr?usp=sharing"; // Replace with your desired URL
            try
            {
                Process.Start(new ProcessStartInfo(url) { UseShellExecute = true });
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Unable to open link. Error: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            exportForm exportForm = new exportForm();
            exportForm.ShowDialog();
        }
    }
}
