#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <MPU6050_tockn.h>

const char* ssid = "felixman";
const char* password = "felixman";
const char* mqtt_server = "broker.hivemq.com";  // 替換成你的 MQTT Broker IP 地址

WiFiClient espClient;
PubSubClient client(espClient);
MPU6050 mpu6050(Wire);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe("esp32/mpu6050");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  mpu6050.update();
  String jsonData = "{\"accX\":" + String(mpu6050.getAccX()) +
                    ",\"accY\":" + String(mpu6050.getAccY()) +
                    ",\"accZ\":" + String(mpu6050.getAccZ()) +
                    ",\"gyroX\":" + String(mpu6050.getGyroX()) +
                    ",\"gyroY\":" + String(mpu6050.getGyroY()) +
                    ",\"gyroZ\":" + String(mpu6050.getGyroZ()) + "}";

  client.publish("esp32/mpu6050", jsonData.c_str());
  delay(1000);  // 每秒傳送一次資料
}
