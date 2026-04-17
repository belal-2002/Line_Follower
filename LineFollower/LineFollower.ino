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

// --- تعريف دبابيس الحساسات الـ 12 ---
  const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
//PD
  bool isRunning = false;

  float Kp = 40.0;
  float Kd = 60.0;

  int baseSpeed = 800;
  int turnSpeed = 800;

  float P = 0;
  float D = 0;
  float lastError = 0;
  float currentError = 0;

//Sensors
  const int sensorWeights[12] = {-55, -45, -35, -25, -15, -5, 5, 15, 25, 35, 45, 55};
  int rawValues[12];  
  long weightedSum = 0;
  long sum = 0;
  bool isLineLost = false;

  //bool leftRadar = false;
  //bool rightRadar = false;



int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

// --- متغيرات نظام التحكم الهجين ---
//enum RobotState { NORMAL_PD, SHARP_LEFT, SHARP_RIGHT };
//RobotState currentState = NORMAL_PD;

bool serviceStarted = false;

void setup() {
  //Serial.begin(115200);
  setupMotors();
  setupSensors();
  setupNetwork();
}

void loop() {
  loopSwitch();
  if (isRunning) {
    calculateError();
    calculatePD();
  } else { 
    if (serviceStarted) {
      loopPrint();
    } else { 
      turnOnService();
    }
  }
}