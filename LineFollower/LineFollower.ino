#include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <TelnetStream.h>
  #include <Wire.h>
  const int MPU_ADDR = 0x68;
  #define SDA_PIN 15  // إضافة رقم الدبوس
  #define SCL_PIN 16  // إضافة رقم الدبوس
  float currentAngleZ = 0.0;
  unsigned long lastMpuTime = 0;
  float gyroZ_offset = 0.0;

// --- إعدادات شبكة الواي فاي ---
  const char* ssid = "Zain_B530_A013";      
  const char* password = "F8BLmiFRedB"; 
  //192.168.8.28

  //const char* ssid = "Galaxy S20+2db9";      
  //const char* password = "55555555";
  //10.189.201.71

// --- تعريف دبابيس المحركات ---
  #define PWMA 42   
  #define AIN1 41   
  #define AIN2 40   
  #define PWMB 19   
  #define BIN1 21   
  #define BIN2 20   
  #define STBY 39   

// ---  تعريف مفتاح التشغيل ودبابيس الاستراتيجيات  ---
  #define limitSwitch 17  
  #define switchPin1 47
  #define switchPin2 48
  #define switchPin3 18
  #define buzzerPin 1

// --- تعريف دبابيس الحساسات الـ 12 ---
  const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
// --- متغيرات حساب المسافة الافتراضية للرادار ---
  float RadarDistanceThreshold = 5.0;   // المسافة المطلوبة بالسنتيمتر (يمكنك تعديلها في أي وقت)
  float distanceNow = 0.0; // المسافة التراكمية الإجمالية التي قطعها الروبوت
  float leftRadarStartDistance = 0.0;   // المسافة المسجلة لحظة التقاط رادار اليسار
  float rightRadarStartDistance = 0.0;  // المسافة المسجلة لحظة التقاط رادار اليمين
  float leftMidRadarStartDistance = 0.0;   
  float rightMidRadarStartDistance = 0.0;  
  float pwmToCmFactor = 0.21;           // معامل تحويل سرعة المحرك (PWM) إلى مسافة (يحتاج لمعايرة بسيطة)
  unsigned long lastDistTime = 0;       // لحساب فرق الوقت dt الخاص بالمسافة حصراً
  int leftSpeed = 0;
  int rightSpeed = 0; 

  static unsigned long lastPrintTime = 0;
  unsigned long turnStartTime = 0;
  unsigned long lastButtonPress = 0;
  const unsigned long debounceDelay = 400;
  bool bit1 = false;
  bool bit2 = false;
  bool bit3 = false;
  byte strategy = 1;
  byte buzzer = 0;
  int sensorBit = 0;
  byte rightSensor = 0;
  byte leftSensor = 0;
  byte midSensor = 0; 
  byte midMidSensor = 0;
  byte allSensor = 0;
  byte radar = 0;
  byte leftRadar = 0;
  byte rightRadar = 0;
  byte leftMidRadar = 0;
  byte rightMidRadar = 0;
  bool leftRadarOn = false;
  bool rightRadarOn = false;
  bool leftMidRadarOn = false;
  bool rightMidRadarOn = false;
  bool allRadarOn = false;
  bool goRight = false;
  bool goLeft = false;
  bool turnLeft = false;
  bool turnRight = false;

//PD
  bool isRunning = false;
  bool serviceStarted = false;

  float Kp = 0.85;
  float Kd = 18.0;
  int maximumSpeed = 625;  //1023;   
  int baseSpeed = 275;  //800;  
  int turnSpeed = 400;  
  int innerTurnSpeed = turnSpeed / 2;  
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

  float P = 0;
  float D = 0;
  float PD_Value =0;
  float lastError = 0;
  float currentError = 0;

  unsigned long currentTime = 0;
  static unsigned long lastTime = 0;
  float dt;

//Error
  const int sensorWeights[12] = {-698, -444, -267, -190, -114, -38, 38, 114, 190, 267, 444, 698};
  int sensorValue[12]; 
  long weightedSum = 0;
  long sum = 0;
  bool lineAvailable = true;
  bool lineWasFound = true;
  



int S_White[12] = {216, 240, 164, 157, 185, 164, 172, 154, 158, 199, 260, 211};
int S_Black[12] = {3375, 1555, 2431, 2106, 2963, 2611, 2639, 2236, 2071, 3135, 3700, 3146};
int target_White = 178;
int target_Black = 2671;
int lineThreshold = 1425;



void setup() {
  //Serial.begin(115200);
  setupMotors();
  setupSwitch();
  setupBuzzer();
  setupSensors();
  setupNetwork();
  setupMPU();
}

void loop() {
  loopSwitch();
  updateDistance();
  updateMPU();
  loopSensors();
  if (isRunning || strategy == 0) {
    loopStrategy();
    } //else {  
    if (serviceStarted) {
      loopPrint();
    } else { 
      turnOnService();
    }
  //}
}
