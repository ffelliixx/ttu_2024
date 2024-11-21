using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UdpServer : MonoBehaviour
{
    public RawImage A; // 主畫面
    public RawImage B; // 場景 B
    public RawImage C; // 場景 C
    public RawImage D; // 場景 D

    private CanvasGroup canvasGroupA;
    private CanvasGroup canvasGroupB;
    private CanvasGroup canvasGroupC;
    private CanvasGroup canvasGroupD;

    private UdpClient udpClient;
    private int port = 16888; // 接收的端口號
    private CancellationTokenSource cancellationTokenSource;
    private string currentMessage = ""; // 儲存收到的訊息

    void Start()
    {
        // 初始化 CanvasGroup
        canvasGroupA = A.GetComponent<CanvasGroup>();
        canvasGroupB = B.GetComponent<CanvasGroup>();
        canvasGroupC = C.GetComponent<CanvasGroup>();
        canvasGroupD = D.GetComponent<CanvasGroup>();

        // 確保 A 場景為主畫面（完全顯示）
        SetCanvasGroupVisibility(canvasGroupA, true);
        SetCanvasGroupVisibility(canvasGroupB, false);
        SetCanvasGroupVisibility(canvasGroupC, false);
        SetCanvasGroupVisibility(canvasGroupD, false);

        // 初始化 UDP Server
        try
        {
            udpClient = new UdpClient(port);
            Debug.Log("UDP Server started on port " + port);
            cancellationTokenSource = new CancellationTokenSource();
            StartReceiving(cancellationTokenSource.Token);
        }
        catch (SocketException e)
        {
            Debug.LogError("Failed to bind to port " + port + ": " + e.Message);
        }
    }

    void Update()
    {
        // 根據收到的訊息處理場景切換
        if (!string.IsNullOrEmpty(currentMessage))
        {
            switch (currentMessage)
            {
                case "BBBB":
                    StartCoroutine(PlayScene(canvasGroupB));
                    break;

                case "CCCC":
                    StartCoroutine(PlayScene(canvasGroupC));
                    break;

                case "DDDD":
                    StartCoroutine(PlayScene(canvasGroupD));
                    break;

                default:
                    Debug.LogWarning("Unknown message received: " + currentMessage);
                    break;
            }

            currentMessage = ""; // 清空已處理的訊息
        }
    }

    private async void StartReceiving(CancellationToken token)
    {
        while (!token.IsCancellationRequested)
        {
            try
            {
                UdpReceiveResult result = await udpClient.ReceiveAsync();
                string message = Encoding.UTF8.GetString(result.Buffer);
                Debug.Log("Received: " + message + " from " + result.RemoteEndPoint);

                // 儲存收到的訊息
                currentMessage = message;
            }
            catch (Exception e)
            {
                Debug.LogError("Error receiving data: " + e.Message);
            }
        }
    }

    private IEnumerator PlayScene(CanvasGroup scene)
    {
        // 淡出主場景
        yield return StartCoroutine(FadeOut(canvasGroupA));

        // 淡入指定場景
        yield return StartCoroutine(FadeIn(scene));

        // 模擬影片播放（10 秒），此處可替換為影片播放完成的事件
        yield return new WaitForSeconds(10);

        // 淡出指定場景
        yield return StartCoroutine(FadeOut(scene));

        // 淡入主場景
        yield return StartCoroutine(FadeIn(canvasGroupA));
    }

    private IEnumerator FadeIn(CanvasGroup canvasGroup)
    {
        float duration = 1.0f; // 淡入持續時間
        float elapsedTime = 0f;

        while (elapsedTime < duration)
        {
            canvasGroup.alpha = Mathf.Lerp(0f, 1f, elapsedTime / duration);
            elapsedTime += Time.deltaTime;
            yield return null;
        }

        canvasGroup.alpha = 1f;
        canvasGroup.interactable = true;
        canvasGroup.blocksRaycasts = true;
    }

    private IEnumerator FadeOut(CanvasGroup canvasGroup)
    {
        float duration = 0.5f; // 淡出持續時間
        float elapsedTime = 0f;

        while (elapsedTime < duration)
        {
            canvasGroup.alpha = Mathf.Lerp(1f, 0f, elapsedTime / duration);
            elapsedTime += Time.deltaTime;
            yield return null;
        }

        canvasGroup.alpha = 0f;
        canvasGroup.interactable = false;
        canvasGroup.blocksRaycasts = false;
    }

    private void SetCanvasGroupVisibility(CanvasGroup canvasGroup, bool visible)
    {
        canvasGroup.alpha = visible ? 1f : 0f;
        canvasGroup.interactable = visible;
        canvasGroup.blocksRaycasts = visible;
    }

    private void OnApplicationQuit()
    {
        if (cancellationTokenSource != null)
        {
            cancellationTokenSource.Cancel();
            cancellationTokenSource.Dispose();
        }
        udpClient.Close();
    }
}
