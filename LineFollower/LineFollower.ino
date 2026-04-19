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

// ---  تعريف مفتاح التشغيل ودبابيس الاستراتيجيات  ---
  #define LIMIT_SWITCH 17  
  #define STRATEGY_1_PIN 18
  #define STRATEGY_2_PIN 47
  #define STRATEGY_3_PIN 48

// --- تعريف دبابيس الحساسات الـ 12 ---
  const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
  unsigned long lastButtonPress = 0;
  const unsigned long debounceDelay = 2500;


//PD
  bool isRunning = false;

  float Kp = 1.15;
  float Kd = 15.0;
  int maximumSpeed = 625;  //1023;   
  int baseSpeed = 275;  //800;  
  int turnSpeed = 600;  //800;  

  float P = 0;
  float D = 0;
  float PD_Value =0;
  float lastError = 0;
  float currentError = 0;

  unsigned long currentTime = 0;
  static unsigned long lastTime = 0;
  float dt;

//Sensors
  //const int sensorWeights[12] = {-465, -296, -178, -127, -76, -25, 25, 76, 127, 178, 296, 465};
  const int sensorWeights[12] = {-698, -444, -267, -190, -114, -38, 38, 114, 190, 267, 444, 698};
  int sensorValue[12];  
  long weightedSum = 0;
  long sum = 0;
  bool lineAvailable = true;
  bool goRight = false;
  //bool goLeft = false;

  int lineThreshold = 1333;


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
  loopSensors();
  if (isRunning) {
    calculateError();
    if (lineAvailable) calculatePD();
      loopMotor();
    } //else { 
    if (serviceStarted) {
      loopPrint();
    } else { 
      turnOnService();
    }
  //}
}