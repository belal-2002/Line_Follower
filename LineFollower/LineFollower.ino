#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

// --- إعدادات شبكة الواي فاي ---
const char* ssid = "Zain_B530_A013";      
const char* password = "F8BLmiFRedB"; 
//192.168.8.28

//const char* ssid = "Galaxy S20+2db9";      
//const char* password = "55555555";
//10.245.67.71

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

// --- تعريف دبابيس وأوزان الحساسات الـ 12 ---
const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};
const int sensorWeights[12] = {-55, -45, -35, -25, -15, -5, 5, 15, 25, 35, 45, 55};

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
bool isRunning = false;

float Kp = 40.0;
//float Ki = 0.001;
float Kd = 60;

float P = 0, I = 0, D = 0, lastError = 0;
float currentError = 0;

int baseSpeed = 800;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

bool serviceStarted = false;
bool lineLost = false; // متغير لمعرفة حالة فقدان الخط

void setup() {
  //Serial.begin(115200);
  // استدعاء دوال الإعداد من الملفات الأخرى
  setupMotors();
  setupSensors();
  setupNetwork();
}

void loop() {
  // 1. فحص زر التشغيل (من ملف Motors)
  checkLimitSwitch();

  // 2. خوارزمية التتبع (من ملفي Sensors و PID)
  if (isRunning) {
    calculateError();
    calculatePID();
  }

  // 3. إرسال البيانات إلى PuTTY (من ملف Network)
  // 4. معالجة الشبكة وأوامر PuTTY (من ملف Network)
  if (serviceStarted) {
    printDebugData();
    handleNetwork();
  } else { 
    TurnOnService();
  }
}