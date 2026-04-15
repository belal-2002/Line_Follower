#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

// --- إعدادات شبكة الواي فاي ---
const char* ssid = "Zain_B530_A013";      
const char* password = "F8BLmiFRedB"; 

// --- تعريف دبابيس المحركات ---
#define PWMA 42   
#define AIN1 41   
#define AIN2 40   
#define PWMB 19   
#define BIN1 21   
#define BIN2 20   
#define STBY 39   

// --- تعريف مفتاح التشغيل ---
#define LIMIT_SWITCH 17  

// --- تعريف دبابيس الحساسات الـ 12 ---
const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// أوزان الحساسات لحساب الخطأ (من أقصى اليسار إلى أقصى اليمين)
const int sensorWeights[12] = {-55, -45, -35, -25, -15, -5, 5, 15, 25, 35, 45, 55};

// --- متغيرات حالة النظام ---
bool isRunning = false;

// --- متغيرات الـ PID والسرعة ---
float Kp = 35.0;  // القيمة الابتدائية (عدلها من PuTTY)
float Ki = 0.005;
float Kd = 0.1;

float P = 0, I = 0, D = 0, lastError = 0;
int baseSpeed = 210; // السرعة الأساسية المعتمدة للاختبار

void setup() {
  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LIMIT_SWITCH, INPUT_PULLUP);
  
  for (int i = 0; i < 12; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  digitalWrite(STBY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.begin();
  TelnetStream.begin();
}

// دالة لمعالجة أوامر الكيبورد من PuTTY
void handleTelnetCommands() {
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    
    // تعديل Kp
    if (c == 'q') Kp += 0.1;
    if (c == 'a') Kp -= 0.1;
    // تعديل Ki
    if (c == 'w') Ki += 0.001;
    if (c == 's') Ki -= 0.001;
    // تعديل Kd
    if (c == 'e') Kd += 0.1;
    if (c == 'd') Kd -= 0.1;

    // منع القيم من النزول تحت الصفر
    if (Kp < 0) Kp = 0;
    if (Ki < 0) Ki = 0;
    if (Kd < 0) Kd = 0;
  }
}

void loop() {
  // 1. الاستماع لتحديثات الهواء وأوامر الكيبورد
  ArduinoOTA.handle();
  handleTelnetCommands();

  // 2. قراءة الحساسات وحساب الخطأ (Error)
  long weightedSum = 0;
  long sum = 0;
  int rawValues[12];
  
  for (int i = 0; i < 12; i++) {
    rawValues[i] = analogRead(sensorPins[i]);
    
    // نأخذ القراءات التي تدل على لون أسود فقط (تجاهل اللون الأبيض والتشويش)
    // القيمة 2000 هي عتبة (Threshold) يمكن تعديلها
    if (rawValues[i] > 2000) {
      weightedSum += (long)rawValues[i] * sensorWeights[i];
      sum += rawValues[i];
    }
  }

  float currentError = 0;
  
  // إذا كان الروبوت يرى الخط، احسب الخطأ
  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    lastError = currentError; // تذكر آخر خطأ
  } else {
    // إذا فقد الخط تماماً، استمر في الدوران بناءً على آخر اتجاه (ذاكرة الروبوت)
    currentError = (lastError > 0) ? 60 : -60; 
  }

  // 3. حساب معادلة الـ PID
  P = currentError;
  I = I + currentError;
  D = currentError - lastError;
  
  float PID_Value = (Kp * P) + (Ki * I) + (Kd * D);

  // 4. تطبيق الـ PID على سرعة المحركات
  int leftMotorSpeed  = baseSpeed + PID_Value;
  int rightMotorSpeed = baseSpeed - PID_Value;

  // تقييد السرعة بين 0 و 255
  if (leftMotorSpeed > 255) leftMotorSpeed = 255;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
  if (rightMotorSpeed > 255) rightMotorSpeed = 255;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;

  // 5. التحكم بالتشغيل والإيقاف (Limit Switch) وإعطاء أوامر للمحركات
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    delay(50); 
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      isRunning = !isRunning; 
      
      if (isRunning) {
        digitalWrite(STBY, HIGH);
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
        digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);  
      } else {
        digitalWrite(STBY, LOW);  
        analogWrite(PWMA, 0);
        analogWrite(PWMB, 0);
        I = 0; // تصفير الخطأ التراكمي عند التوقف
      }
      while(digitalRead(LIMIT_SWITCH) == LOW); 
    }
  }

  if (isRunning) {
    analogWrite(PWMA, leftMotorSpeed);
    analogWrite(PWMB, rightMotorSpeed);
  }

  // 6. إرسال البيانات إلى PuTTY (كل 150 ملي ثانية لكي لا تتجمد الشاشة)
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 1000) {
    // طباعة قيم PID الحالية
    TelnetStream.printf("Kp:%.1f  Ki:%.2f  Kd:%.1f  |  ", Kp, Ki, Kd);
    TelnetStream.println();
    // طباعة الخطأ والسرعات
    //TelnetStream.printf("Err:%4.1f | M_Left:%3d  M_Right:%3d\n", currentError, leftMotorSpeed, rightMotorSpeed);
    
    lastPrintTime = millis();
  }
}