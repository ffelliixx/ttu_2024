#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// WiFi 設定
const char* ssid = "felixman";       // WiFi 名稱
const char* password = "felixman";   // WiFi 密碼

// UDP 設定
WiFiUDP udp;
const char* udpAddress = "192.168.1.230"; // 目標裝置 IP，修改為接收端 IP
const int udpPort = 16888;                // 目標 Port

// MPU6050 設定
Adafruit_MPU6050 mpu;

// 震動閾值 (根據需求調整，單位：m/s²)
#define VIBRATION_THRESHOLD_1 10.5 
#define VIBRATION_THRESHOLD_2 9.5

// 時間變數
unsigned long lastSensorReadTime = 0; // 上一次讀取感測器的時間
unsigned long lastMessageTime = 0;   // 上一次傳送訊息的時間
unsigned long delayDuration = 200;   // 預設傳送間隔 (毫秒)
unsigned long vibrationPause = 15000; // 震動後暫停時間 (毫秒)
bool inPause = false;                 // 是否處於震動暫停狀態

void setup() {
  // Serial 設定
  Serial.begin(115200);

  // WiFi 連線
  WiFi.begin(ssid, password);
  Serial.print("連線中");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi 已連線！");
  Serial.print("IP 位址: ");
  Serial.println(WiFi.localIP());

  // 初始化 MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 初始化失敗！");
    while (1);
  }
  Serial.println("MPU6050 初始化成功！");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("系統已準備好！");
}

void loop() {
  unsigned long currentTime = millis();

  // 確保每 200ms 讀取一次感測器數據
  if (currentTime - lastSensorReadTime >= delayDuration) {
    lastSensorReadTime = currentTime;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // 計算加速度的變化幅度
    float accelerationMagnitude = sqrt(
        a.acceleration.x * a.acceleration.x +
        a.acceleration.y * a.acceleration.y +
        a.acceleration.z * a.acceleration.z);

    String message; // 傳送訊息

    if (inPause) {
      // 暫停期間，發送 "a, accelerationMagnitude"
      message = "a, " + String(accelerationMagnitude);
      Serial.println(message);
    } else {
      // 偵測震動並組成訊息
      if (accelerationMagnitude > VIBRATION_THRESHOLD_1 || accelerationMagnitude < VIBRATION_THRESHOLD_2) {
        message = "AAAA";
        Serial.println(message);

        // 發送 UDP 訊息
        udp.beginPacket(udpAddress, udpPort);
        udp.print(message);
        udp.endPacket();

        // 設定為暫停狀態
        inPause = true;
        lastSensorReadTime = currentTime; // 記錄暫停開始時間
      } else {
        message = "A, " + String(accelerationMagnitude);
        Serial.println(message);
      }
    }

    // 每隔 delayDuration 發送訊息
    udp.beginPacket(udpAddress, udpPort);
    udp.print(message);
    udp.endPacket();

    // 結束暫停狀態
    if (inPause && (currentTime - lastSensorReadTime >= vibrationPause)) {
      inPause = false;
    }
  }
}
