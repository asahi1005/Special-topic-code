// 近距離偵測 (Sensor N)
int const trigPin = 23, echoPin = 22;

int Distance;

void setup() {
  Serial.begin(115200); // 初始化序列通訊 (波特率設為 115200)

  // 距離偵測設定
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  // 測量近距離感測器
  Distance = getDistance(trigPin, echoPin);
  Serial.println(Distance);
  vTaskDelay(500 / portTICK_PERIOD_MS); // ESP32 非阻塞延遲，改用 vTaskDelay 替代 delay()
}

// 測距函數
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
