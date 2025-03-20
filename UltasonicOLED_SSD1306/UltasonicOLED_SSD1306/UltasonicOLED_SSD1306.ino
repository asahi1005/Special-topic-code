#include <Wire.h>
#include <U8g2lib.h>

// **I2C 設定 (OLED)**
#define I2C_SDA 19
#define I2C_SCL 18

// **U8g2 SSD1306 初始化**
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

// **超音波感測器設定**
int const trigPin = 23;
int const echoPin = 22;

unsigned long previousMillis = 0;  // 記錄上次更新時間
const long updateInterval = 3000;  // 更新間隔 (5 秒)

void setup() {
  Serial.begin(115200);

  // **初始化 I2C**
  Wire.begin(I2C_SDA, I2C_SCL);

  // **初始化 OLED (SSD1306)**
  u8g2.begin();
  
  // **顯示初始化訊息**
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB18_tr);  // **顯示 Ready 提示**
  u8g2.setCursor(-5, 20);
  u8g2.print("Ready");
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearBuffer();
  
  // **設定超音波感測器**
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // **每 5 秒才更新一次 OLED**
  if (currentMillis - previousMillis >= updateInterval) {
    previousMillis = currentMillis;  // 更新計時器

    // 取得距離
    int distance = getDistance(trigPin, echoPin);

    // **輸出到 Serial Monitor**
    Serial.print("距離: ");
    Serial.print(distance);
    Serial.println(" cm");

    // **顯示在 OLED 上**
    u8g2.clearBuffer();
    
    // **顯示距離數值**
    u8g2.setFont(u8g2_font_ncenB24_tr);  // **改用 24px 大小**
    int numWidth = u8g2.getUTF8Width(String(distance).c_str()); // **計算數字寬度**
    int numX = (128 - numWidth) / 2;  // **讓數字置中**
    u8g2.setCursor(numX, 40);
    u8g2.print(distance);

    u8g2.sendBuffer(); // 更新 OLED 顯示
  }
}

// **測距函數**
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;  // 計算距離 (cm)
  return distance;
}
