using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using Newtonsoft.Json;
using System.Runtime.CompilerServices;
using System.Threading;
namespace UI
{
    public partial class addFaceForm : Form
    {
        string hostIP = Host.IP;

        // Define the IP address and port
        // Define the IP address and port as readonly fields to avoid referencing non-static fields in initializers
        private readonly IPAddress ipAddress = IPAddress.Parse("192.168.12.2");
        private readonly int port = 5100;

        // Declare the TcpListener without initializing it
        private TcpListener listener;

        // Declare a CancellationTokenSource to manage cancellation
        private CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();
        private CancellationToken _token;
        public addFaceForm()
        {
            InitializeComponent();
            listener = new TcpListener(ipAddress, port);
            _token = _cancellationTokenSource.Token;
        }
        private async Task Listen()
        {
            listener.Start();

            while (!_token.IsCancellationRequested)
            {
                var tcpClient = await listener.AcceptTcpClientAsync();
                await ProcessIncomingConnection(tcpClient);
            }
            Console.WriteLine("Token cancel request");
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
        private byte[] imageToSave;
        private async Task ProcessIncomingConnection(TcpClient tcpClient)
        {
            if (passTextBox.Text == "123456")
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
                    if (delimiterIndex == -1)
                    {
                        Invoke((MethodInvoker)(() =>
                        {
                            label4.Text = "Error during process";
                        }));
                    }
                    else
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
                                if (name == "noface")
                                {
                                    name = "System cannot detect any faces in the picture";
                                }
                                else if (name == "Unknown")
                                {
                                    button2.Show();
                                    name = "Do you want to save this person";
                                    imageToSave = image;
                                }
                                else
                                {
                                    button2.Show();
                                    name = "This person is already saved: " + name;
                                    imageToSave = image;
                                }
                                pictureBox1.Image = tempImage;
                                label4.Text = name;
                            }));
                        }
                    }
                    // convert jpeg uint8 to format that pictureBox can show  

                }
                tcpClient.Close();
            }
            else
            {
                Invoke((MethodInvoker)(() =>
                {
                    label4.Text = "Wrong password!";
                }));
            }
        }
        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        bool savingCapture = true;

        private void button2_Click(object sender, EventArgs e)
        {
            // Send name and image to Python  
            byte[] command = Encoding.UTF8.GetBytes("Save");
            byte splitByte = Encoding.UTF8.GetBytes("?")[0];
            byte[] nameToSave = Encoding.UTF8.GetBytes(textBox1.Text);

            if (imageToSave != null && nameToSave.Length > 0)
            {
                // Combine packet: command + splitByte + nameToSave + imageToSave  
                List<byte> packet = new List<byte>();
                packet.AddRange(command);
                packet.Add(splitByte);
                packet.AddRange(nameToSave);
                packet.Add(splitByte);
                packet.AddRange(imageToSave);

                // Example: Send the packet to a server or process it further  
                // (Replace with actual implementation)  
                Console.WriteLine("Send image with size: " + packet.Count + " to save");
                try
                {
                    TcpClient client = new TcpClient("192.168.12.2", 2300);
                    using (NetworkStream stream = client.GetStream())
                    {
                        stream.Write(packet.ToArray(), 0, packet.Count);
                        Console.WriteLine("Save request successfully.");
                    }
                    client.Close();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error while sending packet: " + ex.ToString());
                }

            }
            else
            {
                MessageBox.Show("Name or image is missing!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        public class Person
        {
            public string name { get; set; }
            public string imagePath { get; set; }
        }
        private void Form2_Load(object sender, EventArgs e)
        {
            button2.Hide();
            _ = Listen();
        }
        string filePath;
        string name_p;
        private void button1_Click(object sender, EventArgs e)
        {
            savingCapture = false;
            if (passTextBox.Text == "123456")
            {
                button2.Hide();
                OpenFileDialog openFileDialog = new OpenFileDialog
                {
                    Title = "Select a picture of the person",
                    Filter = "Image Files (*.png;*.jpg)|*.png;*.jpg|All Files (*.*)|*.*",
                    InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)
                };

                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    filePath = openFileDialog.FileName;
                    name_p = textBox1.Text;
                    button2.Show();
                    try
                    {
                        Image img_ = Image.FromStream(new MemoryStream(File.ReadAllBytes(filePath)));
                        pictureBox1.Image = img_;
                        // Show a rotated image in the PictureBox, but send the original orientation image
                        Image img_s = Image.FromStream(new MemoryStream(File.ReadAllBytes(filePath)));
                        img_s.RotateFlip(RotateFlipType.Rotate180FlipNone);

                        // Send: save the original (unrotated) image to imageToSave
                        using (var ms = new MemoryStream())
                        {
                            img_s.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                            imageToSave = ms.ToArray();
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"Error opening file: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }

            }
            else
            {
                label1.Text = "Wrong password";
                label1.BackColor = Color.Red;
            }
            Console.WriteLine("out");
        }

        private void addFaceForm_FormClosed(object sender, FormClosedEventArgs e)
        {

        }

        private void addFaceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                Byte[] packet = Encoding.UTF8.GetBytes("Exit");
                TcpClient client = new TcpClient("192.168.12.2", 2300);
                using (NetworkStream stream = client.GetStream())
                {
                    stream.Write(packet.ToArray(), 0, packet.Length);
                    Console.WriteLine("Packet sent successfully.");
                }
                client.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while sending packet: " + ex.ToString());
            }
            listener.Stop();
        }
    }
}
