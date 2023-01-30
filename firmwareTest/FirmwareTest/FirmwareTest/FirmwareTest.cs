using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FirmwareTest
{
    public partial class FirmwareTest : Form
    {
        private List<Thread> mWorkerThreads = null;
        public FirmwareTest()
        {
            InitializeComponent();
        }
        public void AddLog(int threadId, string log)
        {
            this.Invoke(new MethodInvoker(() => { edtLog.AppendText($"[{DateTime.Now}][{threadId}]: {log}\r\n"); }));
        }

        private void btnGo_Click(object sender, EventArgs e)
        {
            const int THREAD_COUNT = 3;
            if (mWorkerThreads == null)
            {
                mWorkerThreads = new List<Thread>();
                for (int i = 0; i < THREAD_COUNT; i++)
                {
                    var worker = new Thread(workerThread);
                    worker.Start(i);
                    mWorkerThreads.Add(worker);
                }
            }
        }
        private void workerThread(object parameter)
        {
            int threadId = (int)parameter;

            try
            {

                while (true)
                {
                    AddLog(threadId, "Starting Connection");
                    int sendCount = 10;
                    TcpClient client = new TcpClient();
                    client.Connect(edtIp.Text, 1234);
                    NetworkStream networkStream = client.GetStream();
                    StreamReader reader = new StreamReader(networkStream);
                    StreamWriter writer = new StreamWriter(networkStream);
                    while (sendCount > 0)
                    {
                        sendCount--;
                        if (client.Connected)
                        {
                            writer.Write("GET#");
                            writer.Flush();

                            Thread.Sleep(100);
                            var buffer = new char[512];
                            int length = reader.Read(buffer, 0, buffer.Length);
                            string received = "";
                            for (int i = 0; i < length; i++)
                            {
                                received += buffer[i];
                            }

                            AddLog(threadId, received + "\n");

                            Thread.Sleep(100);
                        }
                    }
                    client.Close();
                    AddLog(threadId, "Done");
                }
            }
            catch (Exception ex)
            {
                AddLog(threadId, ex.Message);
            }

        }
    }
}
