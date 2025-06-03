using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UI
{
    public partial class exportForm : Form
    {
        public exportForm()
        {
            InitializeComponent();
        }

        private void exportForm_Load(object sender, EventArgs e)
        {
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                DateTime startTime = dateTimePicker1.Value;
                DateTime endTime = dateTimePicker2.Value;
                byte[] packet = Encoding.UTF8.GetBytes($"Export?{startTime:dd-MM-yyyy}?{endTime:dd-MM-yyyy}");
                TcpClient client = new TcpClient("192.168.12.2", 2300);
                using (NetworkStream stream = client.GetStream())
                {
                    stream.Write(packet, 0, packet.Length);
                    Console.WriteLine("Packet sent successfully.");
                }
                client.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while sending packet: " + ex.ToString());
            }
        }
    }
}
