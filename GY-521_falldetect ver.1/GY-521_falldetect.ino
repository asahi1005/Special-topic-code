#include <Wire.h>
#include <math.h>

#define SDA_PIN 26
#define SCL_PIN 27
#define MPU6050_ADDR 0x68

// 狀態變數
bool isFalling = false;
bool isStillCounting = false;
unsigned long stillStartTime = 0;
int stillCounter = 0;

// 閾值參數
const float ACC_FALL_THRESHOLD = 0.3;
const float ACC_STILL_MIN = 0.95;
const float ACC_STILL_MAX = 1.05;
const int STILL_TIME_THRESHOLD = 30; // 次數用來避免抖動
const unsigned long STILL_DURATION_LIMIT = 10000; // 10 秒

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  // 初始化 MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("🔧 初始化完成，開始監測...");
}

void loop() {
  int16_t AcX_raw, AcY_raw, AcZ_raw;

  // 讀取加速度原始值
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);

  AcX_raw = Wire.read() << 8 | Wire.read();
  AcY_raw = Wire.read() << 8 | Wire.read();
  AcZ_raw = Wire.read() << 8 | Wire.read();

  // 轉換為 g 值
  float AcX = AcX_raw / 16384.0;
  float AcY = AcY_raw / 16384.0;
  float AcZ = AcZ_raw / 16384.0;

  float totalAcc = sqrt(AcX * AcX + AcY * AcY + AcZ * AcZ);

  // 呼叫偵測函式（內部處理完整邏輯）
  detectFallAndStill(totalAcc);

  Serial.print("Total Acc = ");
  Serial.println(totalAcc, 3);

  delay(100);
}

// 🧠 掉落 + 靜止判斷（含時間限制）
void detectFallAndStill(float totalAcc) {
  if (!isFalling && totalAcc < ACC_FALL_THRESHOLD) {
    isFalling = true;
    isStillCounting = false;
    Serial.println("⚠ 掉落偵測中...");
  }

  if (isFalling) {
    // 檢查是否進入靜止狀態範圍
    if (totalAcc > ACC_STILL_MIN && totalAcc < ACC_STILL_MAX) {
      stillCounter++;

      // 若剛開始靜止，記錄開始時間
      if (!isStillCounting && stillCounter > 5) {
        isStillCounting = true;
        stillStartTime = millis();
        Serial.println("🕒 開始靜止計時...");
      }

      // 若已經靜止夠久，輸出警告
      if (isStillCounting && millis() - stillStartTime >= STILL_DURATION_LIMIT) {
        Serial.println("⚠ 已掉落並靜止超過 10 秒！");
        // 重置所有狀態
        isFalling = false;
        isStillCounting = false;
        stillCounter = 0;
      }

    } else {
      // 有晃動就重置所有靜止判斷
      stillCounter = 0;
      isStillCounting = false;
    }
  }
}
