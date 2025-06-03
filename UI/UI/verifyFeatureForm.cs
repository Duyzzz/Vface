using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using Newtonsoft.Json.Linq;

namespace UI
{
    public partial class verifyFeatureForm : Form
    {
        // Define the IP address and port
        // Define the IP address and port as readonly fields to avoid referencing non-static fields in initializers
        private readonly IPAddress ipAddress = IPAddress.Parse("192.168.12.2");
        private readonly int port = 5100;

        // Declare the TcpListener without initializing it
        private TcpListener listener;

        // Declare a CancellationTokenSource to manage cancellation
        private CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();
        private CancellationToken _token;

        // Initialize the TcpListener in the constructor or a method
        public verifyFeatureForm()
        {
            InitializeComponent();
            listener = new TcpListener(ipAddress, port);
            _token = _cancellationTokenSource.Token;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // request cancellation
            _cancellationTokenSource.Cancel();
            listener.Stop();
            Console.WriteLine("Token cancel request");
        }
        

        private async Task Listen()
        {
            try
            {
                listener.Start();
                while(!_token.IsCancellationRequested)
                {
                    Console.WriteLine("Listening!");
                    var tcpClient = await listener.AcceptTcpClientAsync();
                    await ProcessIncomingConnection(tcpClient);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"The client closed, stop listening. Error: {ex.Message}");
                this.Close();
            }
        }
        private int FindSequence(List<byte> source, byte[] sequence)
        {
            for (int i = 0; i <= source.Count - sequence.Length; i++)
            {
                bool match = true;
                for (int j = 0; j < sequence.Length; j++)
                {
                    if (source[i + j] != sequence[j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                {
                    return i;
                }
            }
            return -1; // Return -1 if the sequence is not found  
        }
        // Define the ProcessIncomingConnection method
        private async Task ProcessIncomingConnection(TcpClient tcpClient)
        {
            // Example implementation for processing the connection  
            using (NetworkStream stream = tcpClient.GetStream())
            {
                byte[] buffer = new byte[1024];
                int totalBytes = 0;
                List<byte> total = new List<byte>();
                int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                while (bytesRead > 0)
                {
                    totalBytes += bytesRead;
                    total.AddRange(buffer.Take(bytesRead));
                    bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                }
                Console.WriteLine("Received " + total.Count.ToString() + " bytes");
                byte[] ackcBytes = Encoding.UTF8.GetBytes("ackc");
                int delimiterIndex = FindSequence(total, ackcBytes);
                if(delimiterIndex == -1)
                {
                    Invoke((MethodInvoker)(() =>
                    {
                        recognizeHumanLabel.Text = "Failing in receiving packet";
                    }));
                }else
                {
                    byte[] nameBytes = total.GetRange(0, delimiterIndex).ToArray(); // Fix CS0029 by converting List<byte> to byte[]
                    string name = Encoding.UTF8.GetString(nameBytes); // Fix CS1001, CS1002, and CS0128 by completing the statement and avoiding duplicate variable names
                    byte[] image = total.GetRange(delimiterIndex + ackcBytes.Length, totalBytes - delimiterIndex - ackcBytes.Length).ToArray();
                    using (var ms = new MemoryStream(image))
                    {
                        // Ensure UI updates are performed on the UI thread  
                        Invoke((MethodInvoker)(() =>
                        {
                            Image tempImage = Image.FromStream(ms);
                            tempImage.RotateFlip(RotateFlipType.Rotate180FlipNone);
                            pictureBox1.Image = tempImage;
                            if(name == "noface")
                            {
                                name = "System cannot see any faces in the picture";
                            }
                            recognizeHumanLabel.Text = name;
                            recognizeHumanLabel.AutoSize = true;
                            recognizeHumanLabel.Location = new Point(
                                (this.ClientSize.Width - recognizeHumanLabel.Width) / 2,
                                recognizeHumanLabel.Location.Y
                            );
                            recognizeHumanLabel.BringToFront();
                            recognizeHumanLabel.Visible = true; // Ensure the label is visible
                        }));
                    }
                }
                // convert jpeg uint8 to format that pictureBox can show  
                
            }
            tcpClient.Close();
        }
        private void verifyFeatureForm_Load(object sender, EventArgs e)
        {
            recognizeHumanLabel.Hide();
            pictureBox1.Image = Image.FromFile("D:\\university\\ky8_last\\DATN\\prjCsharp\\UI\\backgroundImage\\board_circle.jpg");
            _ = Listen();
        }

        private void verifyFeatureForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            _cancellationTokenSource.Cancel();
            listener.Stop();
            Console.WriteLine("Token cancel request");
        }
    }
}
