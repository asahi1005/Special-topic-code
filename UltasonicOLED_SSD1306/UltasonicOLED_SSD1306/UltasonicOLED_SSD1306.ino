#include <Wire.h>
#include <U8g2lib.h>

// **I2C 設定 (OLED)**
#define I2C_SDA 19
#define I2C_SCL 18

// **U8g2 SSD1306 初始化**
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

// **超音波感測器設定**
const int trigPin = 23;
const int echoPin = 22;

unsigned long previousMillis = 0;
const long updateInterval = 500;  // 每 0.5 秒更新

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  u8g2.begin();

  // 初始畫面顯示 "Ready"
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB18_tr);
  u8g2.setCursor(-5, 20);
  u8g2.print("Ready");
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearBuffer();

  // 設定腳位
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= updateInterval) {
    previousMillis = currentMillis;

    int distance = getAverageDistance(trigPin, echoPin, 5);  // 取 5 次平均

    // 排除異常值 (太小或太大)
    if (distance > 2 && distance < 400) {
      Serial.print("距離: ");
      Serial.print(distance);
      Serial.println(" cm");

      // 顯示到 OLED
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB24_tr);
      int numWidth = u8g2.getUTF8Width(String(distance).c_str());
      int numX = (128 - numWidth) / 2;
      u8g2.setCursor(numX, 40);
      u8g2.print(distance);
      u8g2.sendBuffer();
    } else {
      Serial.println("測距異常，忽略此次資料");
    }
  }
}

// 單次測距
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

// 多次平均距離
int getAverageDistance(int trigPin, int echoPin, int samples) {
  long sum = 0;
  int validCount = 0;

  for (int i = 0; i < samples; i++) {
    int d = getDistance(trigPin, echoPin);
    if (d > 2 && d < 400) {  // 只計算有效數值
      sum += d;
      validCount++;
    }
    delay(50);
  }

  if (validCount == 0) return 0;
  return sum / validCount;
}
