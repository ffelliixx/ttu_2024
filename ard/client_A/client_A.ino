#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "felixman";
const char *pwd = "felixman";
const char *udpAddress = "192.168.3.10";
const int udpPort = 16888;

WiFiUDP udp;

Adafruit_MPU6050 mpu;

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  Serial.begin(115200);
  
  //setup wifi
  WiFi.begin(ssid, pwd);
  while (WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.println("Connecting");
      }
    Serial.println("Connected!");
  
  //MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G: Serial.println("+-2G"); break;
  case MPU6050_RANGE_4_G: Serial.println("+-4G"); break;
  case MPU6050_RANGE_8_G: Serial.println("+-8G"); break;
  case MPU6050_RANGE_16_G: Serial.println("+-16G"); break;
  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG: Serial.println("+- 250 deg/s"); break;
  case MPU6050_RANGE_500_DEG: Serial.println("+- 500 deg/s"); break;
  case MPU6050_RANGE_1000_DEG: Serial.println("+- 1000 deg/s"); break;
  case MPU6050_RANGE_2000_DEG: Serial.println("+- 2000 deg/s"); break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ: Serial.println("260 Hz"); break;
  case MPU6050_BAND_184_HZ: Serial.println("184 Hz"); break;
  case MPU6050_BAND_94_HZ: Serial.println("94 Hz"); break;
  case MPU6050_BAND_44_HZ: Serial.println("44 Hz"); break;
  case MPU6050_BAND_21_HZ: Serial.println("21 Hz"); break;
  case MPU6050_BAND_10_HZ: Serial.println("10 Hz"); break;
  case MPU6050_BAND_5_HZ: Serial.println("5 Hz"); break;
  }
  udp.beginPacket(udpAddress, udpPort);
  udp.write((const uint8_t *)"A_finish_setup", strlen("A_finish_setup"));
  udp.endPacket();
  delay(100);
}

void loop() {
  //clock
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  //mpu6050data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //wifi checking
  if (WiFi.status() == WL_CONNECTED) {
    char buffer[128];
      snprintf(buffer, sizeof(buffer), 
               "(A)%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
               a.acceleration.x, a.acceleration.y, a.acceleration.z, 
               g.gyro.x, g.gyro.y, g.gyro.z, temp.temperature); 
    udp.beginPacket(udpAddress, udpPort); 
    udp.write((const uint8_t *)buffer, strlen(buffer));
    udp.endPacket();
    }
  }
}
