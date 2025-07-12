/*
  Connect:
  NodeMCU    -> Matrix
  MOSI-D7-GPIO13  -> DIN
  CLK-D5-GPIO14   -> CK
  GPIO0-D3        -> CS
*/
#include <ESP8266WiFi.h>
const char* ssid = "Khoa";
const char* password = "10121999";

#include <WiFiUdp.h>
#include <NTPClient.h>
WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org");
//NTPClient timeClient(ntpUDP, "2.asia.pool.ntp.org");
//NTPClient timeClient(ntpUDP, "ntp.aliyun.com");
NTPClient timeClient(ntpUDP, "192.168.1.22");

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
int pinCS = 0;                                                                                    // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;                                                               //số trong Hiển thị theo chiều ngang
int numberOfVerticalDisplays = 4;                                                                 // Số trong hiển thị theo chiều dọc.
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);  // cấu hình matrix

void setup() {
  Serial.begin(115200);
  delay(200);

  matrix.setIntensity(1);  // cài đặt giá trị độ tương phản từ 0 đến 15.
  matrix.setRotation(3);   // The same hold for the last display

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int kn = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    kn = !kn;

    matrix.fillScreen(LOW);
    int y = (matrix.height() - 8) / 2;
    int xh = 5;

    String s = "WIFI";
    if (kn) {
      matrix.drawChar(xh, y, s[0], HIGH, LOW, 1);
      matrix.drawChar(xh + 6, y, s[1], HIGH, LOW, 1);
      matrix.drawChar(xh + 2 * 6, y, s[2], HIGH, LOW, 1);
      matrix.drawChar(xh + 3 * 6, y, s[3], HIGH, LOW, 1);
    } else {
      matrix.drawChar(xh, y, (String(" "))[0], HIGH, LOW, 1);
      matrix.drawChar(xh + 6, y, (String(" "))[0], HIGH, LOW, 1);
      matrix.drawChar(xh + 2 * 6, y, (String(" "))[0], HIGH, LOW, 1);
      matrix.drawChar(xh + 3 * 6, y, (String(" "))[0], HIGH, LOW, 1);
    }
    matrix.write();
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  timeClient.begin();
  timeClient.setTimeOffset(25200);
  timeClient.setUpdateInterval(60000);  // Cập nhật NTP mỗi 1phút/lần
}

uint32_t TIME_RUN = 0;
int dem_get_time = 10;
bool haicham = 0;

int gio, gio_ol = -1;
int phut, phut_ol = -1;
int giay, giay_ol = -1;

void loop() {
  if (millis() - TIME_RUN > 500) {
    TIME_RUN = millis();

    haicham = !haicham;
    matrix.fillScreen(LOW);
    int y = (matrix.height() - 8) / 2;
    if (haicham) {
      matrix.drawChar(14, y, (String(":"))[0], HIGH, LOW, 1);
    } else {
      matrix.drawChar(14, y, (String(" "))[0], HIGH, LOW, 1);
    }

    dem_get_time++;
    if (dem_get_time >= 2) {
      dem_get_time = 0;
      if (timeClient.update()) {
        gio = timeClient.getHours();
        phut = timeClient.getMinutes();
        giay = timeClient.getSeconds();
      } else {
        timeClient.begin();
        timeClient.setTimeOffset(25200);
      }
    }
  }

  if (gio != gio_ol || phut != phut_ol) {
    int xh = 2;
    int xm = 19;
    int y = (matrix.height() - 8) / 2;
    String hour1 = String(gio / 10);
    String hour2 = String(gio % 10);
    String min1 = String(phut / 10);
    String min2 = String(phut % 10);
    matrix.drawChar(xh, y, hour1[0], HIGH, LOW, 1);
    matrix.drawChar(xh + 6, y, hour2[0], HIGH, LOW, 1);
    matrix.drawChar(xm, y, min1[0], HIGH, LOW, 1);
    matrix.drawChar(xm + 6, y, min2[0], HIGH, LOW, 1);
    matrix.write();
  }
}
