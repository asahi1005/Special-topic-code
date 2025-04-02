#include <Wire.h>
#include <U8g2lib.h>

// OLED I2C 設定
#define I2C_SDA 19
#define I2C_SCL 18

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

// 超音波模組腳位
const int trigPin = 23;
const int echoPin = 22;

const unsigned long measureInterval = 50;   // 每 50ms 偵測一次
const unsigned long displayInterval = 500;  // 每 500ms 顯示平均
unsigned long lastMeasureTime = 0;
unsigned long lastDisplayTime = 0;

const int sampleSize = 10;
int distanceSamples[sampleSize];
int sampleIndex = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB18_tr);
  u8g2.setCursor(-5, 20);
  u8g2.print("Ready");
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearBuffer();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // 每 50ms 偵測一次距離
  if (currentMillis - lastMeasureTime >= measureInterval) {
    lastMeasureTime = currentMillis;
    recordDistance();
  }

  // 每 500ms 顯示平均距離
  if (currentMillis - lastDisplayTime >= displayInterval) {
    lastDisplayTime = currentMillis;
    displayAverageDistance();
  }
}

// ✅ 封裝：記錄一次距離到陣列
void recordDistance() {
  int d = getDistance(trigPin, echoPin);

  if (d > 2 && d < 400) {
    distanceSamples[sampleIndex] = d;
  } else {
    distanceSamples[sampleIndex] = -1; // 無效值
  }

  sampleIndex = (sampleIndex + 1) % sampleSize;
}


// ✅ 封裝：計算平均並顯示
void displayAverageDistance() {
  int sum = 0;
  int count = 0;
  for (int i = 0; i < sampleSize; i++) {
    if (distanceSamples[i] != -1) {
      sum += distanceSamples[i];
      count++;
    }
  }

  if (count > 0) {
    int avgDistance = sum / count + 1;

    Serial.print("平均距離: ");
    Serial.print(avgDistance);
    Serial.println(" cm");

    // 顯示在 OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB24_tr);
    int numWidth = u8g2.getUTF8Width(String(avgDistance).c_str());
    int numX = (128 - numWidth) / 2;
    u8g2.setCursor(numX, 40);
    u8g2.print(avgDistance);
    u8g2.sendBuffer();
  } else {
    Serial.println("無有效資料");
  }
}

// ✅ 測距函數
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}
