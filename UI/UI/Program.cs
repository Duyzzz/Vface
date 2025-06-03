using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace UI
{
    //string hostIP = "192.168.217.149"; // Leduy
    //string hostIP = "192.168.1.162"; //Anhson
    //string hostIP = "192.168.242.149"; // Doan
    public static class Host{ // python
        public const string IP = "192.168.12.2";
        public const int port = 1400;
    }
    public static class Esp32
    {
        public const string IP = "192.168.12.1";
        public const int port = 8080;
    }
    public static class Winform
    {
        public const string IP = "192.168.12.2";
        public const int port = 5100;
    }
    internal static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new mainForm());
        }
    }
}
