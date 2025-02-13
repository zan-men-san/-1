#include <WiFi.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

// WiFi 设置
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// NTP 客户端
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// WS2812C 灯珠设置
#define PIN 27 // 连接到GPIO 27
#define NUMPIXELS 120 // 总灯珠数量
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// 定义颜色
#define COLOR_OFF 0x000000
#define COLOR_ON 0xFF0000 // 红色示例，可根据需要修改

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show(); // 初始化所有灯珠为关闭状态

  // 连接到WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // 初始化NTP客户端
  timeClient.begin();
  timeClient.setTimeOffset(28800); // 设置时区为东八区，可根据需要调整
}

void loop() {
  timeClient.update();
  displayTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  delay(1000); // 每秒更新一次
}

void displayTime(int hour, int minute, int second) {
  clearStrip(); // 清除所有灯珠

  // 显示小时
  displayDigit(hour / 10, 0, 10); // 十位
  displayDigit(hour % 10, 20, 30); // 个位

  // 显示分钟
  displayDigit(minute / 10, 40, 50); // 十位
  displayDigit(minute % 10, 60, 70); // 个位

  // 显示秒
  displayDigit(second / 10, 80, 90); // 十位
  displayDigit(second % 10, 100, 110); // 个位

  strip.show(); // 更新灯珠显示
}

void displayDigit(int digit, int startPixel, int startPixel2) {
  for (int i = 0; i < 10; i++) {
    int pixel = startPixel + i;
    int pixel2 = startPixel2 + i;
    if (i == digit) {
      strip.setPixelColor(pixel, COLOR_ON);
      strip.setPixelColor(pixel2, COLOR_ON);
    } else {
      strip.setPixelColor(pixel, COLOR_OFF);
      strip.setPixelColor(pixel2, COLOR_OFF);
    }
  }
}

void clearStrip() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, COLOR_OFF);
  }
}