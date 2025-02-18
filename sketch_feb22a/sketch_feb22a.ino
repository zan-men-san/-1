
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LED_PIN         5
#define NUM_LEDS        120
#define BUTTON_PIN      4

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi配置
const char* ssid = "Redmi K70 Ultra";
const char* password = "eauvyx3qppriwyu";

// NTP客户端配置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 8*3600); // 时区偏移（秒）

// 颜色配置
uint32_t colors[3] = {
  strip.Color(255, 0, 0),   // 红色
  strip.Color(0, 0, 255),   // 蓝色
  strip.Color(0, 255, 0)    // 绿色
};
volatile uint8_t colorIndex = 0;
volatile bool colorChangeFlag = false;

// 时间变量
int hours = 0;
int minutes = 0;
int seconds = 0;

void IRAM_ATTR handleButton() {
  colorChangeFlag = true;
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show(); // 初始化灯带

  // 按钮初始化
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, FALLING);

  // 连接WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // 初始化NTP客户端
  timeClient.begin();
  updateTimeFromNTP();
}

void loop() {
  static unsigned long lastUpdate = 0;
  
  // 处理颜色切换
  if (colorChangeFlag) {
    colorIndex = (colorIndex + 1) % 3;
    updateDisplay();
    colorChangeFlag = false;
    delay(200); // 简易防抖
  }

  // 每秒更新时间
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    updateTime();
    updateDisplay();

    // 每小时同步一次时间
    static unsigned long lastSync = 0;
    if (millis() - lastSync >= 3600000) {
      updateTimeFromNTP();
      lastSync = millis();
    }
  }
}

void updateTimeFromNTP() {
  timeClient.forceUpdate();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  hours = ptm->tm_hour;
  minutes = ptm->tm_min;
  seconds = ptm->tm_sec;
}

void updateTime() {
  seconds++;
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours >= 24) {
        hours = 0;
      }
    }
  }
}

void updateDisplay() {
  strip.clear();
  
  // 显示小时（0-39）
  displayDigit(hours / 10, 0);   // 十位
  displayDigit(hours % 10, 20);  // 个位

  // 显示分钟（40-79）
  displayDigit(minutes / 10, 40); // 十位
  displayDigit(minutes % 10, 60); // 个位

  // 显示秒（80-119）
  displayDigit(seconds / 10, 80);  // 十位
  displayDigit(seconds % 10, 100); // 个位
  
  strip.show();
}

void displayDigit(int number, int baseAddr) {
  number = constrain(number, 0, 9);
  strip.setPixelColor(baseAddr + number, colors[colorIndex]);
  strip.setPixelColor(baseAddr + number + 10, colors[colorIndex]);
}