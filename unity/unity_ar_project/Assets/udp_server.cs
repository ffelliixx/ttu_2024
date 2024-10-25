using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

public class UDPServer : MonoBehaviour
{
    private UdpClient udpServer;
    private Thread receiveThread;
    private int port = 16888;

    void Start()
    {
        udpServer = new UdpClient(port);
        receiveThread = new Thread(new ThreadStart(ReceiveData));
        receiveThread.IsBackground = true; 
        receiveThread.Start();
        Debug.Log("UDP Server started on port " + port);
    }

    void ReceiveData()
    {
        IPEndPoint remoteEndPoint = new IPEndPoint(IPAddress.Any, port);
        while (true)
        {
            try
            {
                
                byte[] receivedBytes = udpServer.Receive(ref remoteEndPoint);
                string receivedData = Encoding.UTF8.GetString(receivedBytes);
                Debug.Log("Received data from " + remoteEndPoint + ": " + receivedData);
            }
            catch (SocketException e)
            {
                Debug.LogError(e.ToString());
            }
        }
    }

    void OnApplicationQuit()
    {
        receiveThread.Abort();
        udpServer.Close();
        Debug.Log("UDP Server stopped.");
    }
}
